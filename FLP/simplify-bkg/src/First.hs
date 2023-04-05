-- Project : simplify-bkg
-- Author  : Peter Grofcik
-- Login   : xgrofc00
-- Year    : 2020
-- Algoritmus 4.1 - L(G) != {} -> ?? search for Nt of nonterminas that generate terminals && if reachable from start
module First where
  import Data.List.Split
  import Syntax

  -- function to obtain Nt using recursion
  -- called from obtainBKG1 to obtain Nt (can be empty if there are no terminals genereted or
  -- if any cant be reached from start -> that speeds up process of other functions even for obtainBKG2
  -- because it would mean that it will be empty after next step anyway)
  -- that afterwards creates an grammar that generates empty language G({S}, {}, S, {S->#}) - S represents starting nonterminal
  obtainNt :: [String] -> BKG -> [String]
  obtainNt [] bkg = []
  obtainNt n bkg = do
    let n1 = obtainNi [] (n ++ sig bkg) (p bkg)
    if n1 /= n then
      obtainNt n1 bkg
    else
      n1
      -- if check n1 [(s bkg)] then     --acording to FLP forum this should not be done here but later for step 2
      --   n1
      -- else
      --   []

  -- function that generates N_i set that stands later as a new Nt set if it is different from the last one (N_i-1) in obtainNt function
  -- it goes trough all rules in BKG and finds new nonterminals (on left side) that can reach any from N_i-1 united with sigma
  obtainNi :: Foldable t => [String] -> t String -> [String] -> [String]
  obtainNi [] n0 [] = []
  obtainNi n1 n0 [] = n1
  obtainNi [] n0 p =
    if check (concat n0) (splitOn "->" (head p)!!1) then
      obtainNi [head (splitOn "->" (head p))] n0 (tail p)
    else
      obtainNi [] n0 (tail p)
  obtainNi n1 n0 p =
    if check (concat n0) (splitOn "->" (head p)!!1) &&
      not(check (concat n1) (head (splitOn "->" (head p)))) then
        obtainNi (n1 ++ [head (splitOn "->" (head p))]) n0 (tail p)
    else
      obtainNi n1 n0 (tail p)

  -- function that creates new grammar from obtained nt set
  -- if obtained nt is empty, as described before, it creates grammar that generates empy language
  -- also uses ontainPK function to select rules that consts of set Nt united with sigma only
  obtainBKG1 :: BKG -> BKG
  obtainBKG1 bkg = do
    let n0 = obtainNi [] (sig bkg) (tail (p bkg))-- initial call of obtainNi reduces structural ifs in "obtainNt [] bkg"
    let nt = obtainNt n0 bkg -- call wiith an empty n0 will be done only if the input grammar generates an empty language
    let pk = obtainPK (nt ++ sig bkg) [] (p bkg)
    if not(check nt [s bkg]) then
      BKG False (nt ++ [s bkg]) (sig bkg) (s bkg) pk -- false indicates empy grammar for step 2
    else
      BKG True nt (sig bkg) (s bkg) pk -- not actualy specified if sig should be reduced at this stage

  -- function that goes trough all the rulles of loaded BKG and creates new PK set without rules that contain terminals that are not in Nt set
  obtainPK  :: Foldable t => t String -> [String] -> [String] -> [String]
  obtainPK nt pk [] = pk
  obtainPK nt [] p =
    if check (concat nt) (head (splitOn "->" (head p))) &&
       check (concat nt) (splitOn "->" (head p)!!1) then
      obtainPK nt [head p] (tail p)
    else
      obtainPK nt [] (tail p)

  obtainPK nt pk p =
    if check (concat nt) (head (splitOn "->" (head p))) &&
       check (concat nt) (splitOn "->" (head p)!!1) then
      obtainPK nt ( pk ++ [head p]) (tail p)
    else
      obtainPK nt pk (tail p)
