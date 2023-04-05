# import random
import logging
import numpy as np
from os.path import dirname

from dicewars.client.ai_driver import BattleCommand, EndTurnCommand


class AI:

    def __init__(self, player_name, board, players_order):
        self.board = board
        self.player_name = player_name
        self.logger = logging.getLogger('AI')
        self.players = board.nb_players_alive()
        self.largest_region = []

        self.handle = np.loadtxt(dirname(__file__) + '/weights')
        self.weigths = [[0 for a in range(8)] for a in range(8)]
        self.weigths_t = [[0 for a in range(8)] for a in range(8)]

        count = 0
        rows = 0
        testing = 0
        for row in self.handle:
            if row[0] == -2:
                testing += 1
                if rows != 0:
                    for i in range(8):
                        counter = 0
                        for column in self.handle[rows-8+i]:
                            if column != -1:
                                self.weigths_t[i][counter] += 1
                                self.weigths[i][counter] += column
                            counter += 1
                count += 1
            rows += 1

        # for w in self.weigths:
        #     print(w)
        #     print('\n')
        #
        #
        # for w in self.weigths_t:
        #     print(w)
        #     print('\n')


        index1 = 0
        for row in self.weigths:
            index2 = 0
            for column in row:
                if self.weigths_t[index1][index2] != 0:
                    self.weigths[index1][index2] = column/self.weigths_t[index1][index2]
                index2 += 1
            index1 += 1

        self.weigthsAVG = 0
        for row in self.weigths:
            for column in row:
                self.weigthsAVG += column
        self.weigthsAVG = self.weigthsAVG/64

        # for w in self.weigths:
        #     print(w)
        #     print('\n')
        # print(self.weigths)

        self.players_order = players_order
        while self.player_name != self.players_order[0]:
            self.players_order.append(self.players_order.pop(0))



    def ai_turn(self, board, nb_moves_this_turn, nb_turns_this_game, time_left):
        self.board = board
        turns = self.possible_attacks()

        can_be_attacked = []
        for turn in turns:
            can_be_attacked.append(turn[1])

        attacks = []
        regions = self.board.get_players_regions(self.player_name)
        regions = sorted(regions, key=lambda region: len(region), reverse=True)
        for region in regions:
            for area in region:
                for turn in turns:
                    if turn[0] == area:
                        for adj in self.board.get_area(turn[1]).get_adjacent_areas():
                            if self.board.get_area(adj).get_owner_name() == self.player_name and adj not in region:
                                attacks.append([turn, len(region)+1+len(self.board.get_areas_region(adj, self.board.get_player_areas(self.player_name)))
                                                + 0 if self.board.get_area(turn[0]).get_dice() != 8 else len(self.board.areas)
                                                # + self.board.get_area(turn[0]).get_dice()
                                                ])
                            elif self.board.get_area(adj).get_owner_name() != self.player_name and adj in can_be_attacked:
                                for can in self.board.get_area(adj).get_adjacent_areas():
                                    if self.board.get_area(can).get_owner_name() == self.player_name and can not in region:
                                        attacks.append([turn, len(region)+2+len(self.board.get_areas_region(can, self.board.get_player_areas(self.player_name)))
                                                        # + self.board.get_area(turn[0]).get_dice()
                                                        ])

        attack = sorted(attacks, key=lambda turn: turn[1], reverse=True)

        # Tattack = []
        # if attack :
        #     for simple in attack:
        #         if simple[0][1]

        select_attacks = []
        if attack:
            for at in attack:
                if at[1] == attack[0][1]:
                    select_attacks.append(at[0])
            select_attacks = sorted(select_attacks, key=lambda turn: turn[2], reverse=True)


        if select_attacks:
            return BattleCommand(select_attacks[0][0], select_attacks[0][1])
        # elif best_turns:
        #     return BattleCommand(best_turns[0][0], best_turns[0][1])
        else:
            backfire = self.possible_backfire()
            best_turns = []
            self.get_largest_region()
            for turn in backfire:
                for area in self.largest_region:
                    if turn[0] == area:
                        best_turns.append(turn)
                        break
            best_turns = sorted(best_turns, key=lambda turn: turn[2], reverse=True)

            if best_turns:
                return BattleCommand(best_turns[0][0], best_turns[0][1])
            elif backfire:
                return BattleCommand(backfire[0][0], backfire[0][1])
            else:
                if len(self.board.get_player_areas(self.player_name)) < len(self.board.areas)/2:
                    risk = self.risky_attacks()
                    if risk:
                        return BattleCommand(risk[0][0], risk[0][1])

        self.logger.debug("Don't want to attack anymore.")
        return EndTurnCommand()


    def get_largest_region(self):
        self.largest_region = []

        players_regions = self.board.get_players_regions(self.player_name)
        max_region_size = max(len(region) for region in players_regions)
        max_sized_regions = [region for region in players_regions if len(region) == max_region_size]

        for region in max_sized_regions:
            for area in region:
                self.largest_region.append(area)
        return max_region_size


    def possible_attacks(self):
        turns = []
        for area in self.board.get_player_areas(self.player_name):
            if not area.can_attack():
                continue
            neighbours = area.get_adjacent_areas()

            for adj in neighbours:
                adjacent_area = self.board.get_area(adj)
                if adjacent_area.get_owner_name() != self.player_name:
                    area_name = area.get_name()
                    atk = area.get_dice()
                    defend = adjacent_area.get_dice()

                    if self.weigths[atk-1][defend-1] > self.weigthsAVG or atk is 8:
                            weigth = self.weigths[atk-1][defend-1] if atk != 8 else atk
                            turns.append([area_name, adj, weigth])

        return sorted(turns, key=lambda turn: turn[2], reverse=True)

    def possible_backfire(self):
        turns = []
        for area in self.board.get_player_areas(self.player_name):
            if not area.can_attack():
                continue
            neighbours = area.get_adjacent_areas()

            for adj in neighbours:
                adjacent_area = self.board.get_area(adj)
                if adjacent_area.get_owner_name() != self.player_name:
                    area_name = area.get_name()
                    atk = area.get_dice()
                    defend = adjacent_area.get_dice()

                    attack = True
                    if self.weigths[atk-1][defend-1] > self.weigthsAVG or atk is 8:
                        for backfire in adjacent_area.get_adjacent_areas():
                            backfire_area = self.board.get_area(backfire)
                            if backfire_area.get_owner_name() != self.player_name:
                                if self.weigths[backfire_area.get_dice()-1][atk-1] > self.weigths[atk-1][defend-1]:
                                    attack = False
                        if attack or atk is 8:
                            weigth = self.weigths[atk-1][defend-1] if atk != 8 else atk
                            turns.append([area_name, adj, weigth])

        return sorted(turns, key=lambda turn: turn[2], reverse=True)

    def risky_attacks(self):
        turns = []
        for area in self.board.get_player_areas(self.player_name):
            if not area.can_attack():
                continue
            neighbours = area.get_adjacent_areas()

            for adj in neighbours:
                adjacent_area = self.board.get_area(adj)
                if adjacent_area.get_owner_name() != self.player_name:
                    area_name = area.get_name()
                    atk = area.get_dice()
                    defend = adjacent_area.get_dice()
                    if atk < 8:
                        atk += 1 # risk choice for bigger attacking propability
                    # if defend > 1:
                    #     defend -= 1

                    if self.weigths[atk-1][defend-1] > self.weigthsAVG or atk is 8:
                        weigth = self.weigths[atk-1][defend-1] if atk != 8 else atk
                        turns.append([area_name, adj, weigth])

        return sorted(turns, key=lambda turn: turn[2], reverse=True)




