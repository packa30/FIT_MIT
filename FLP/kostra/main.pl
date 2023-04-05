%  Project : Graph Sceleton
%  Course	: Functional and Logic Programming
%  Author  : Peter Grofcik
%  Login   : xgrofc00
%  Year    : 2020

%%%%% Parsing input file predicates from (provided solution) %%%%%
read_line(L,C) :-
	get_char(C),
	(isEOFEOL(C), L = [], !;
		read_line(LL,_),
		[C|LL] = L).

isEOFEOL(C) :-
	C == end_of_file;
	(char_code(C,Code), Code==10).

read_lines(Ls) :-
	read_line(L,C),
	( C == end_of_file, Ls = [] ;
	  read_lines(LLs), Ls = [L|LLs]
	).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% corrects list with edge (unwanted chars)
correct_line([],[]) :- !.
correct_line([' '|T], S1) :- !, correct_line(T,S1).
correct_line([32|T], S1) :- !, correct_line(T,S1).
correct_line([H|T], [H|S1]) :- correct_line(T,S1).

% correction of all lists inside a list of list to correct all single edges
correct_lines([],[]).
correct_lines([L|Ls],[H|T]) :- correct_lines(Ls,T), correct_line(L,H).

% provides a single all row list from list of lists
unifiedch([],[]) :- !.
unifiedch([[]|T], S1) :- unifiedch(T,S1).
unifiedch([[H|G]|T], [H|S1]) :- unifiedch([G|T],S1).

% provides list with only unique points
remove_dups([], []).
remove_dups([H|T], T2) :- member(H, T), remove_dups(T, T2).
remove_dups([H|T], [H|T2]) :- not(member(H, T)), remove_dups(T, T2).

% carrier predicate true/false if a desired edge exists in input parsed edges
% used in both bfs & dfs predicates when creating Sceletons to determine wandering over a tree from one point
edge(_, _, []) :- false.
edge(X, Y, [[X|[Y|_]]|_]).
edge(X, Y, [_|T]) :- edge(X,Y,T).

% connection can be both ways (simple or w/reverse)
connected(X,Y,Lines) :- edge(X,Y,Lines) ; edge(Y,X,Lines).

appen([], X, X).
appen([H|T], X, [H|S]) :- appen(T, X, S).
appen(H, T, [H|T]). % addition to buildin append that provides append for 2 single points also

%%%%%%%%%%%%% checking if loaded graph is continuous %%%%%%%%%%%%%
% functionality : starting from one point traverse trough simulated tree to all possible points
% 			example :	A -> B : B -> C,D : C->G,H & D ->L (only to not visited one)
% 			storing : all points from previous and current level
% 			  fail	: if not traversed trough all points on input
bfs([],_,_,[],_,[]).% ends up when ToBeVisited is empty

bfs([],OneRowUnique,Lines,ToBeVisited,Visited,T2) :-% end of checking current level -> continue with the next one (ToBeVisited)
	bfs(ToBeVisited,OneRowUnique,Lines,ToBeVisited,Visited,T2).

bfs([H|T],OneRowUnique,Lines,[_|ToBeVisited],Visited,[H|T2]) :-% all other levels can have more points to check for next row
	bfscounter(H,OneRowUnique,Lines,Bfsrow),
	appen(Visited,[H],Visit),
	bfschecks(Bfsrow,Visit,Next),
	bfschecks(Next,ToBeVisited,Next2),
	append(ToBeVisited,Next2,ToBeVisited2),
	bfs(T,OneRowUnique,Lines,ToBeVisited2,Visit,T2).

bfs(X,OneRowUnique,Lines,ToBeVisited,Visited,[X|T]) :-% at the beginning theres only one point X
	bfscounter(X,OneRowUnique,Lines,Bfsrow),
	append(Visited,X,Visit),
	bfschecks(Bfsrow,Visit,Next),
	bfschecks(Next,ToBeVisited,Next2),
	append(ToBeVisited,Next2,ToBeVisited2),
	bfs(ToBeVisited2,OneRowUnique,Lines,ToBeVisited2,Visit,T).

bfscounter(X, [H|T], Lines, [H|T2]) :- connected(X,H,Lines),bfscounter(X,T,Lines,T2).
bfscounter(X, [H|T], Lines, T2) :- not(connected(X,H,Lines)),bfscounter(X,T,Lines,T2).
bfscounter(_, [], _, []).

bfschecks([],_,[]).
bfschecks([H|T],[],[H|T2]) :- bfschecks(T,[],T2).
bfschecks([H|T],Visit,[H|T2]) :- not(member(H,Visit)),bfschecks(T,Visit,T2).
bfschecks([H|T],Visit,Next) :- member(H,Visit),bfschecks(T,Visit,Next).

checkM([],_).
checkM([H|T],Gen) :- member(H,Gen),checkM(T,Gen).%simple check if list of all loaded points are in AllReached points list
checkM([H|_],Gen) :- not(member(H,Gen)),false.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%%%%%%%%%% DFS based method for finding all possible Sceletons
% cycling for a value of points-1, because in a worst case scenario there is a single sceleton on input
% on each level generates all possible combinations where edges leads from previous one
dfs(_,_,_,[],_,StaticEdges) :- writeOut([],StaticEdges).%special case optimization2 when input is Sceleton and nothing else can be done
dfs(0,FinalSceletons,_,_,_,StaticEdges) :- duplicitDeletion(FinalSceletons,WOduplicit),output(WOduplicit,StaticEdges).

dfs(Counter,[],OneRowUnique,Lines,Sceletons,StaticEdges) :-
	NextCounter is Counter - 1,
	dfs(NextCounter,Sceletons,OneRowUnique,Lines,[],StaticEdges).

dfs(Counter,[H|T],OneRowUnique,Lines,Sceletons,StaticEdges) :-
	dfscounter(H,OneRowUnique,Lines,SceletonsAdd),
	append(Sceletons,SceletonsAdd,SceletonsAdded),
	dfs(Counter,T,OneRowUnique,Lines,SceletonsAdded,StaticEdges).

dfs(Counter,X,OneRowUnique,Lines,Sceletons,StaticEdges) :-
	bfscounter(X,OneRowUnique,Lines,Bfsrow),
	createEdges(X,Bfsrow,RowScelet),
	NextCounter is Counter-1,
	dfs(NextCounter,RowScelet,OneRowUnique,Lines,Sceletons,StaticEdges).

% for each point int current Sceleton (coresponding to level) finds all combinations where it might go next and generates Sceletons for all that
dfscounter(X,OneRowUnique,Lines,NextSceletons) :-
	unifiedch(X,RowOfE),
	remove_dups(RowOfE,RowUnique),
	dfsgen(X,RowUnique,RowUnique,OneRowUnique,Lines,[],NextSceletons).

% for a signle point find all possibilities to go next, but if a point before in sceleton or were already visited do not add them to possibilities
dfsgen(X,[],_,_,_,PossibleAdds,NextSceletons) :- createNextScel(X,PossibleAdds,NextSceletons).
dfsgen(X,[H|T],RowUnique,OneRowUnique,Lines,PossibleAdds,NextSceletons) :-
	bfscounter(H,OneRowUnique,Lines,Row),
	bfschecks(Row,RowUnique,Next),
	dfsEdges(H,Next,AddSkel),
	append(PossibleAdds,AddSkel,NextSkel),
	dfsgen(X,T,RowUnique,OneRowUnique,Lines,NextSkel,NextSceletons).

% Helping predicates to work with a list of Sceletons (eg. list of list of lists)
createEdges(_,[],[]).
createEdges(X,[H|T],[[H2]|T2]) :-
	appen([X],[H],H2),
	createEdges(X,T,T2).

dfsEdges(_,[],[]).
dfsEdges(X,[H|T],[H2|T2]) :-
	appen([X],[H],H2),
	dfsEdges(X,T,T2).

createNextScel(_,[],[]).
createNextScel(X,[H|T],[H2|T2]) :-
	append(X,[H],H2),
	createNextScel(X,T,T2).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%  predicates to be executed after DFS computation is done to remove any generated duplicits in a list of Sceletons
duplicitDeletion([],[]).
duplicitDeletion([H|T],[H|T2]) :- not(checkremaining(H,T)),duplicitDeletion(T,T2).
duplicitDeletion([H|T],Rest) :- checkremaining(H,T),duplicitDeletion(T,Rest).

checkremaining(_,[]) :- false.
checkremaining(Sceleton,[H|T]) :- not(checkSceleton(Sceleton,H)),checkremaining(Sceleton,T).
checkremaining(Sceleton,[H|_]) :- checkSceleton(Sceleton,H),true.

checkSceleton([],_) :- true.
checkSceleton([[H|[G|_]]|T],Sceleton) :- dCheckSingle(H,G,Sceleton),checkSceleton(T,Sceleton).
checkSceleton([[H|[G|_]]|_],Sceleton) :- not(dCheckSingle(H,G,Sceleton)),false.

dCheckSingle(_,_, []) :- false.
dCheckSingle(X,Y, [H|T]) :- not(member(X,H)),dCheckSingle(X,Y,T).
dCheckSingle(X,Y, [H|T]) :- not(member(Y,H)),dCheckSingle(X,Y,T).
dCheckSingle(X,Y, [H|_]) :- member(X,H),member(Y,H),true.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% list of lists (list of Sceletons) on input
% generates output from iner representation
output([],_).
output([H|T],StaticEdges) :- writeOut(H,StaticEdges),output(T,StaticEdges).%static edges carry over from optimization2
writeOut([H|T],StaticEdges) :- atomic_list_concat(H,'-',Out),write(Out),write(' '),writeOut(T,StaticEdges).
writeOut([],[H|T]) :- atomic_list_concat(H,'-',Out),write(Out),write(' '),writeOut([],T).
writeOut([],[]) :- put(10).

% first part that just parse input and generates list of list with loaded edges & list of points in loaded edges (unique)
extraction(Rules,OneRowUnique) :-
	prompt(_, ''),
	read_lines(Lines),
	correct_lines(Lines,Rules),%corrects empy line chars in lists
	% Lines is a list of lists
	% (to obtain just list and also a list with unique points)
	unifiedch(Rules,LinesOneRow),
	remove_dups(LinesOneRow,OneRowUnique).

% execution to (maybe) provide graph sceletons
execution(Rules,OneRowUnique) :-
	nth0(0, OneRowUnique, Start),
	bfs(Start,OneRowUnique,Rules,[],[],AllReached),
	length(OneRowUnique,Length),
	Counter is Length-1,
	optimalization2(Counter,Rules,[],RulesFinal,StaticEdgesFinal),
	optimalization1(RulesFinal,Furthest),%% found furthest point from point within the most count of edges
	findSceletons(Furthest,RulesFinal,OneRowUnique,AllReached,StaticEdgesFinal).

findSceletons(_,_,OneRowUnique,AllReached,_) :-
	not(checkM(OneRowUnique,AllReached)).%special ending in case if checkM fails means that there is no continuous graph at input (if not dfs can not fail)
findSceletons(Start,Rules,OneRowUnique,AllReached,StaticEdges) :-
	checkM(OneRowUnique,AllReached),
	% optimization2 adding need new OneRowUnique for actual edges as well as new Counter
	unifiedch(Rules,LinesOneRow),
	remove_dups(LinesOneRow,OneRowUniqueNEW),
	length(OneRowUniqueNEW,Length),
	Counter is Length-1,
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	dfs(Counter,Start,OneRowUniqueNEW,Rules,[],StaticEdges).%provide (search for) sceletons only if the graph is continuous
	% StaticEdges are just carry over until end of dfs to be added to all generated Sceletons staticaly


%%%%%%%%%%%%%%%%%%%%% Optimization part (saves cost & time consumption) %%%%%%%%%%%%%%%%%%%%%
%% based on finding the best starting point for DFS alg.
%% if we accidentaly start from point with most edges attached it will generate most possibilities from the first level of dfs,
%% but if we can find the furthest point from this point we can minimalize possibilities at the beginning and possibly
%% generate biggest bulk at the end of alg. that will save cost in memory (less duplicits) -> saved time

optimalization1(Rules,Last) :-
	unifiedch(Rules,LinesOneRow),
	remove_dups(LinesOneRow,OneRowUnique),
	findMostEdgesPoint(OneRowUnique,LinesOneRow,Rules,Costs),
	findBiggest(Costs,0,0,Winner),
	findFurthest(Winner,OneRowUnique,Rules,Last).

findFurthest(Winner,OneRowUnique,Rules,Last) :-
	bfs(Winner,OneRowUnique,Rules,[],[],AllReached),%%last in AllReached is the furthest one from our Winner
	obtainLast(AllReached,Last).

% obtain last list
obtainLast([Last],Last).
obtainLast([_|T],Last) :- obtainLast(T,Last).

% finds point that is in the most count of edges
findBiggest([],Winner,_,Winner).
findBiggest([[H,N]|T],_,N2,Winner) :- N > N2,findBiggest(T,H,N,Winner).
findBiggest([[_,N]|T],H2,N2,Winner) :- N =< N2,findBiggest(T,H2,N2,Winner).

% finds counts of edges for each point
findMostEdgesPoint([],_,_,[]).
findMostEdgesPoint([H|T],LinesOneRow,Rules,[[H,N]|T2]) :-
	count(H,LinesOneRow,N),
	findMostEdgesPoint(T,LinesOneRow,Rules,T2).

% finds how many times an X is in input list
count(_,[],0).
count(X,[X|T],N) :-
  count(X,T,N1),
  N is N1 + 1.
count(X, [_|T], N) :-
  count(X, T, N).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Second one is base on removing static edges that will always be a part of Sceletons generated
% find any of edges with point that is not in any other and remove that edge from Rulse -> saving it in StaticEdges
% this is done in a cycle for the cound of points, because removing one might mean that another Point that was in 2 edges
% is now also only in one and can be removed as well
% afterwards execution of DFS only on non statical edges is way more faster and less data consuming
% at the end simply adds all found StaticEdes to generated Sceletons
% optimization 1 should take place after this one, because of changed input edges might provide other best starting point
optimalization2(0,Rules,StrippedRules,RulesFinal,StaticEdgesFinal) :- generate(Rules,RulesFinal),generate(StrippedRules,StaticEdgesFinal).
optimalization2(Counter,Rules,StrippedRules,RulesFinal,StaticEdgesFinal) :-
	unifiedch(Rules,LinesOneRow),
	remove_dups(LinesOneRow,OneRowUnique),
	findMostEdgesPoint(OneRowUnique,LinesOneRow,Rules,Costs),
	selPointsW1Edge(Costs,PointsW1Edge),
	% write(Rules).
	deletePointsW1Edge(PointsW1Edge,Rules,StripRule),
	append(StrippedRules,StripRule,NextStrippedRules),
	% write(NextStrippedRules).
	stripRules(StripRule,Rules,FinalRules),
	CounterNext is Counter-1,
	optimalization2(CounterNext,FinalRules,NextStrippedRules,RulesFinal,StaticEdgesFinal).

generate([],[]).
generate([H|T],[H|T2]) :- generate(T,T2).

stripRules([],Rules,FinalRules) :- generate(Rules,FinalRules).
stripRules([H|T],Rules,FinalRules) :-
	stripRule(H,Rules,NextRules),
	stripRules(T,NextRules,FinalRules).

stripRule(_,[],[]).
stripRule([X,Y],[H|T],T2) :- member(X,H),member(Y,H),stripRule([[X|Y]|_],T,T2).
stripRule([X,Y],[H|T],[H|T2]) :- stripRule([X,Y],T,T2).

% generates list of points with 1 single connection
selPointsW1Edge([[H,1]|T],[H|T2]) :- selPointsW1Edge(T,T2).
selPointsW1Edge([[_,_]|T],T2) :- selPointsW1Edge(T,T2).
selPointsW1Edge([],[]).

% generates a list of edges with a point with 1 single connection
deletePointsW1Edge([],_,[]).
deletePointsW1Edge([H|T],Rules,[H2|T2]) :-
	deletePoint(H,Rules,H2),
	deletePointsW1Edge(T,Rules,T2).

deletePoint(X,[H|T],StaticEdge) :- not(member(X,H)),deletePoint(X,T,StaticEdge).
deletePoint(X,[StaticEdge|_],StaticEdge) :- member(X,StaticEdge).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

main :-
		% stack limits needs to be increased ONLY for no optimizated solution in given tests not final solution,but better be sure than sorry
		% set_prolog_stack(global, limit(1 000 000 000)),
		% set_prolog_stack(local, limit(1 000 000 000)),
		extraction(Rules,OneRowUnique),
		execution(Rules,OneRowUnique),
		halt.
