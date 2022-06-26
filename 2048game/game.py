import random
import copy

# 盤面のグリッド数
SIZE = 4


class Game():
    """
    2048 game main logic

    Attributes:
        board list : state of game
        score : total game score
    """

    def __init__(self, input=None):
        self.board = [row[:] for row in input or [[0] * SIZE]*SIZE]
        self.score = 0

    def show(self):
        """
        print current game board

        Args:None
        Returns:None
        """
        bar = '-' * ((SIZE+3) * 4 + 1)
        print(bar)
        for row in self.board:
            print(
                '| ' + ' | '.join(f'{tile:4}' for tile in row) + ' |')
        print(bar)

    def rotate(self, times=1):
        """
        rotate game board clockwise

        Args:
            times : number of rotation
        Returns:None
        """
        for i in range(times):
            self.board = [list(row)[::-1] for row in zip(*self.board)]

    def move_left(self):
        """
        simulate board when left was selected and returns True if any grid are moved

        Args:None
        Returns:
            moved (bool): whether board was moved or not
        """
        moved = False
        for row in self.board:
            for left in range(SIZE-1):
                for right in range(left+1, SIZE):
                    if row[right] == 0:
                        continue
                    if row[left] == 0:
                        # in this case row[right] is not 0
                        row[left] = row[right]
                        row[right] = 0
                        moved = True
                        continue
                    if row[left] == row[right]:
                        # in this case there is no 0s between row[left] and row[right]
                        self.score += 2*row[left]
                        row[left] = 2*row[left]
                        row[right] = 0
                        moved = True
                        break
                    if row[left] != row[right]:
                        break
        return moved

    def move_right(self):
        """
        simulate board when right was selected and returns True if any grid are moved

        Args:None
        Returns:
            moved (bool): whether board was moved or not
        """
        self.rotate(2)
        moved = self.move_left()
        self.rotate(2)
        return moved

    def move_up(self):
        """
        simulate board when up was selected and returns True if any grid are moved

        Args:None
        Returns:
            moved (bool): whether board was moved or not
        """
        self.rotate(3)
        moved = self.move_left()
        self.rotate(1)
        return moved

    def move_down(self):
        """
        simulate board when down was selected and returns True if any grid are moved

        Args:None
        Returns:
            moved (bool): whether board was moved or not
        """
        self.rotate(1)
        moved = self.move_left()
        self.rotate(3)
        return moved

    def playable(self):
        """
        judge whether any tile can be placed or not

        Args:None
        Returns:
            is_ok (bool): flag whether ok or not
        """

        return any(move_d(Game(copy.deepcopy(self.board)))
                   for move_d in (Game.move_left, Game.move_right,
                                  Game.move_up, Game.move_down))

    def put_tile(self):
        """
        put tile randomly in grids which are not occupied
        probability of occurrence 2 or 4 is 90% and 10%

        Args:
            None
        Returns:
            None
        """
        zeros = [(x, y) for x in range(SIZE)
                 for y in range(SIZE) if self.board[x][y] == 0]
        x, y = random.choice(zeros)
        val = random.choices((2, 4), weights=(9, 1))[0]
        self.board[x][y] = val


def play():
    game = Game()
    game.put_tile()
    game.put_tile()
    game.show()
    key_flick = {'r': game.move_right,
                 'l': game.move_left,
                 'u': game.move_up,
                 'd': game.move_down}
    try:
        count = 0
        while game.playable():
            key = input(f'input {count}th move>>> ')
            if key not in key_flick:
                print('err')
            elif key_flick[key]():
                game.put_tile()
                game.show()
                print(f'score = {game.score}')
                count += 1
    except (KeyboardInterrupt, EOFError):
        print()
    print('#######################')
    print('Game Over')
    print(f'Final Score = {game.score}')
    print('#######################')


if __name__ == '__main__':
    play()