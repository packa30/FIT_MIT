from os.path import dirname
from random import shuffle
import logging
import numpy

from dicewars.ai.utils import possible_attacks, probability_of_holding_area
from dicewars.ai.utils import probability_of_successful_attack, sigmoid

from dicewars.client.ai_driver import BattleCommand, EndTurnCommand


class AI:
    """Agent using Win Probability Maximization (WPM) using logarithms
    of player scores and dice

    This agent estimates win probability given the current state of the game.
    As a feature to describe the state, a vector of logarithms of players' dice
    and scores is used. The agent choses such moves, that will have the highest
    improvement in the estimated probability.
    """

    def __init__(self, player_name, board, players_order):
        """
        Parameters
        ----------
        game : Game
        """
        self.board = board
        self.player_name = player_name
        self.logger = logging.getLogger('AI')
        self.attacks = [[0 for col in range(8)] for row in range(8)]
        self.holds = [[0 for col in range(8)] for row in range(8)]
        self.hp = [[0 for yep in range(3)] for area in board.areas]
        self.finish = [[-2 for col in range(8)] for row in range(8)]
        self.handle = open(dirname(__file__) + '/weights', 'a')
        for a in range(8):
            self.handle.write('-2 ')
        self.handle.write('\n')
        self.handle.flush()

    def ai_turn(self, board, nb_moves_this_turn, nb_turns_this_game, time_left):
        """AI agent's turn

        Get a random area. If it has a possible move, the agent will do it.
        If there are no more moves, the agent ends its turn.
        """
        self.board = board
        idx = 0
        own_area = board.get_player_areas(self.player_name)
        for area in self.hp:
            if area[0] == 1:
                for own in own_area:
                    if own.get_name() == idx+1:
                        self.holds[area[1] - 1][area[2] - 1] += 1
                area[0] = 0
                area[1] = 0
                area[2] = 0
            idx += 1


        idx1 = 0
        for (at, ho, fin) in zip(self.attacks, self.holds, self.finish):
            idx2 = 0
            for (a, h, f) in zip(at, ho, fin):
                if a == 0:
                    self.finish[idx1][idx2] = -1
                elif h == 0:
                    self.finish[idx1][idx2] = 0
                else:
                    self.finish[idx1][idx2] = h / a
                idx2 += 1
            idx1 += 1



        attacks = list(possible_attacks(board, self.player_name))

        # if attacks.__len__() <= 1:
        for fin in self.finish:
            counter = 0
            for f in fin:
                if counter != 7:
                    self.handle.write(str(f) + ' ')
                else:
                    self.handle.write(str(f))
                counter += 1
            self.handle.write('\n')
        self.handle.flush()

        turns = self.possible_turns()
        if turns:
            turn = turns[0]
            source = turn[0]
            target = turn[1]
            self.attacks[source.get_dice() - 1][target.get_dice() - 1] += 1
            self.hp[target.get_name() - 1][0] = 1
            self.hp[target.get_name() - 1][1] = source.get_dice()
            self.hp[target.get_name() - 1][2] = target.get_dice()

            return BattleCommand(source.get_name(), target.get_name())
        #
        # for source, target in attacks:
        #     self.attacks[source.get_dice() - 1][target.get_dice() - 1] += 1
        #     self.hp[target.get_name() - 1][0] = 1
        #     self.hp[target.get_name() - 1][1] = source.get_dice()
        #     self.hp[target.get_name() - 1][2] = target.get_dice()
        #     return BattleCommand(source.get_name(), target.get_name())

        self.logger.debug("No more possible turns.")
        return EndTurnCommand()

    def possible_turns(self):
        turns = []
        for source, target in possible_attacks(self.board, self.player_name):
            area_name = source.get_name()
            atk_power = source.get_dice()
            atk_prob = probability_of_successful_attack(self.board, area_name, target.get_name())
            hold_prob = atk_prob * probability_of_holding_area(self.board, target.get_name(), atk_power - 1, self.player_name)
            if hold_prob >= 0.2 or atk_power == 8:
                turns.append([source, target, hold_prob])

        return sorted(turns, key=lambda turn: turn[2], reverse=True)