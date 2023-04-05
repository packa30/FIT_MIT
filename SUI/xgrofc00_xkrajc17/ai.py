import logging

import dicewars.ai.utils as util

from dicewars.ai.xgrofc00_xkrajc17.loader import Loader
from dicewars.client.ai_driver import BattleCommand, EndTurnCommand


class AI:

    def __init__(self, player_name, board, players_order):
        self.board = board
        self.player_name = player_name
        self.logger = logging.getLogger('AI')
        self.players = board.nb_players_alive()
        self.largest_region = []

        self.weights, self.weightsAVG = Loader().load_trainer_weights()

        self.ownAreasCount = len(self.board.get_player_areas(self.player_name))
        self.allAreasCount = len(self.board.areas)

        self.best_turns = []
        self.good_turns = []
        self.turns = []
        self.risky_turns = []
        self.turns_region = []

        self.count = 0
        self.players_order = players_order
        while self.player_name != self.players_order[0]:
            self.players_order.append(self.players_order.pop(0))

    def empty_turns(self):
        self.best_turns = []
        self.good_turns = []
        self.turns = []
        self.risky_turns = []
        self.turns_region = []

    def get_possible_turns(self, area, neighbors):
        for adj in neighbors:
            adjacent_area = self.board.get_area(adj)
            if adjacent_area.get_owner_name() != self.player_name:
                weight = self.weights[area.get_dice()-1][adjacent_area.get_dice()-1] if area.get_dice() != 8 else 1
                weight_back = 0.0
                for backfire in adjacent_area.get_adjacent_areas():
                    backfire_area = self.board.get_area(backfire)
                    if backfire_area.get_owner_name() != self.player_name:
                        w = self.weights[backfire_area.get_dice()-1][area.get_dice()-1]
                        if w > weight_back:
                            weight_back = w
                self.turns.append([area.get_name(), adj, weight, weight_back])

        self.turns = sorted(self.turns, key=lambda turn: turn[2], reverse=True)

    def sort_turns(self):
        for turn in self.turns:
            if (turn[2] > self.weightsAVG > turn[3]) or (turn[2] == 1 and self.weightsAVG > turn[3]):
                self.best_turns.append(turn)
            if turn[2] > self.weightsAVG or turn[2] == 1: #turn[3]:
                self.good_turns.append(turn)
            if turn[2] + 0.2 > self.weightsAVG or turn[2] == 1:
                self.risky_turns.append(turn)

        self.best_turns = sorted(self.best_turns, key=lambda best_turn: best_turn[2], reverse=True)
        self.good_turns = sorted(self.good_turns, key=lambda good_turn: good_turn[2], reverse=True)

    def get_new_size(self, turn, turn_ex):
        weight = 0
        for region in self.board.get_players_regions(self.player_name):
            for area in region:
                if area == turn[0] or area == turn_ex[0]:
                    weight += len(region)
        return weight

    def get_con_back(self, desired, atk, turn, count):
        found = 100     # should never be reached -> stays if not found connection
        adj_areas = self.board.get_area(turn).get_adjacent_areas()
        if atk > 1:
            for adj in adj_areas:
                if adj == desired:
                    return count
                elif self.weights[atk-1][self.board.get_area(adj).get_dice()-1] > self.weightsAVG \
                        and adj != turn \
                        and self.board.get_area(adj).get_owner_name() != self.player_name:
                    nx_found = self.get_con_back(desired, atk-1, adj, count+1)
                    if found > nx_found:
                        found = nx_found
        return found

    def get_con(self, turn, actual_turn, atk, turns, count):
        if atk > 1:
            for turn_ex in turns:
                if actual_turn[1] == turn_ex[1]:
                    self.turns_region.append([turn, self.get_new_size(turn, turn_ex), count])

                found = self.get_con_back(actual_turn[1], self.board.get_area(turn_ex[0]).get_dice()-1, turn_ex[1], 0)
                if found < 100:
                    self.turns_region.append([turn, self.get_new_size(turn, turn_ex), count + found])

                for adj in self.board.get_area(actual_turn[1]).get_adjacent_areas():
                    if self.weights[atk-1][self.board.get_area(adj).get_dice()-1] > self.weightsAVG\
                            and adj != actual_turn[0]\
                            and self.board.get_area(adj).get_owner_name() != self.player_name:
                        self.get_con(turn, [actual_turn[1], adj, self.weights[atk-1][self.board.get_area(adj).get_dice()-1]], atk-1, turns, count+1)

    def get_attacks_based_on_regions(self, turns):
        regions = sorted(self.board.get_players_regions(self.player_name), key=lambda region: len(region), reverse=True)
        self.turns_region = []
        if regions.__len__() > 1:
            for turn in turns:
                to_expel = []
                for region in regions:
                    for area in region:
                        if area == turn[0]:
                            to_expel = region

                new_turns = []
                for tu in turns:
                    if tu[0] not in to_expel:
                        new_turns.append(tu)

                self.get_con(turn, turn, self.board.get_area(turn[0]).get_dice()-1, new_turns, 0)

        turns_region = sorted(self.turns_region, key=lambda turns_r: turns_r[2], reverse=False)
        self.turns_region = []
        for turn in turns_region:
            if turn[2] == turns_region[0][2]:
                self.turns_region.append(turn)

        turns_region = sorted(self.turns_region, key=lambda turns_r: turns_r[1], reverse=True)
        self.turns_region = []
        for turn in turns_region:
            if turn[1] == turns_region[0][1]:
                self.turns_region.append(turn)

        self.turns_region = sorted(self.turns_region, key=lambda turns_r: turns_r[0][2], reverse=True)

    def get_attacks(self):
        # empty turns from last TURN
        self.empty_turns()

        for area in self.board.get_player_areas(self.player_name):
            if not area.can_attack():
                continue
            self.get_possible_turns(area, area.get_adjacent_areas())

        self.sort_turns()

    def get_attack_clusters(self, attacks):
        attacks_final = []
        clusters_from_area = [[] for area in self.board.areas]
        clusters_to_area = [[] for area in self.board.areas]
        clusters_to_final = [[] for area in self.board.areas]
        clusters_from_final = [[] for area in self.board.areas]
        for attack in attacks:
            clusters_from_area[attack[0]-1].append(attack)
            clusters_to_area[attack[1]-1].append(attack)

        attacks_todo = []
        for cluster in clusters_from_area:
            if cluster:
                attacks_from = cluster[0]
                for at in cluster:
                    if attacks_from[2] > at[2]:
                        attacks_from = at
                attacks_todo.append(attacks_from)

        attacks_todo = sorted(attacks_todo, key=lambda attack_todo: attack_todo[2], reverse=False)
        for attack in attacks_todo:
            clusters_to_final[attack[1] - 1] = attack

        for attack in clusters_to_final:
            if attack:
                clusters_from_final[attack[0]-1] = attack

        # print(clusters_to_final)
        for attack in attacks:
            if not clusters_from_final[attack[0]-1]:    # and not clusters_to_final[attack[1]-1]:
                if clusters_to_final[attack[1]-1]:
                    if clusters_to_final[attack[1] - 1][2] < attack[2]:
                        clusters_to_final[attack[1]-1] = attack
                else:
                    clusters_to_final[attack[1]-1] = attack

        for attack in clusters_to_final:
            if attack:
                attacks_final.append(attack)

        attacks_final = sorted(attacks_final, key=lambda attack_final: attack_final[2], reverse=True)

        return attacks_final

    def possible_co_turn(self, turns):
        new_turns = []
        final_new = []
        for player in self.players_order:
            if self.player_name != player:
                regions = sorted(self.board.get_players_regions(player), key=lambda region: len(region), reverse=True)
                for turn in turns:
                    if turn[1] in regions[0] and len(regions[0]) > 1:
                        for area in regions[0]:
                            if area != turn[1]:
                                start_area = area
                                break
                        new_region = self.find_connections(start_area, turn[1], player)
                        if len(new_region) + 1 != len(regions[0]):
                            len_region = len(regions[0]) - len(new_region)
                            todo_turn = [turn, len_region >= len(new_region) if len_region else len(new_region)] # choose biggest region from split
                            new_turns.append(todo_turn)
        new_turns = sorted(new_turns, key=lambda new_turn: new_turn[1], reverse=False)
        for new in new_turns:
            if new[1] == new_turns[0][1]:
                final_new.append(new[0])
        return sorted(final_new, key=lambda final: final[2], reverse=True)

    def find_connections(self, start_area, ex_area, player):
        new_region = [start_area]
        for check in new_region:
            for adj in self.board.get_area(check).get_adjacent_areas():
                adjacent_area = self.board.get_area(adj)
                if adjacent_area.get_owner_name() == player and adj not in new_region and adj != ex_area:
                    new_region.append(adj)
        return new_region

    def ai_turn(self, board, nb_moves_this_turn, nb_turns_this_game, time_left):
        self.board = board      # new board
        self.ownAreasCount = len(self.board.get_player_areas(self.player_name))
        self.allAreasCount = len(self.board.areas)
        self.get_attacks()

        if not time_left:
            util.save_state(self.count, self.board, self.player_name, self.players_order)
            self.count += 1

        if self.best_turns:
            self.get_attacks_based_on_regions(self.best_turns)
            co_turn = self.possible_co_turn(self.best_turns)
            attacks = self.get_attack_clusters(self.best_turns)
            if self.turns_region:
                return BattleCommand(self.turns_region[0][0][0], self.turns_region[0][0][1])
            elif co_turn:
                return BattleCommand(co_turn[0][0], co_turn[0][1])
            elif attacks:
                return BattleCommand(attacks[0][0], attacks[0][1])
        elif self.good_turns:
            self.get_attacks_based_on_regions(self.good_turns)
            co_turn = self.possible_co_turn(self.good_turns)
            attacks = self.get_attack_clusters(self.good_turns)
            if self.turns_region:
                return BattleCommand(self.turns_region[0][0][0], self.turns_region[0][0][1])
            elif nb_moves_this_turn < self.ownAreasCount/self.players:
                if co_turn:
                    return BattleCommand(co_turn[0][0], co_turn[0][1])
                # elif attacks:
                #     return BattleCommand(attacks[0][0], attacks[0][1])
        elif self.risky_turns and self.ownAreasCount < self.allAreasCount/(self.players + 1):
            self.get_attacks_based_on_regions(self.risky_turns)
            if self.turns_region:
                return BattleCommand(self.turns_region[0][0][0], self.turns_region[0][0][1])
        #     elif nb_moves_this_turn < self.ownAreasCount/self.players:
        #         return BattleCommand(self.risky_turns[0][0], self.risky_turns[0][1])

        self.logger.debug("Don't want to attack anymore.")
        return EndTurnCommand()


