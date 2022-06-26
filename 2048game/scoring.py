from solver import Solver
from game import Game


res = Solver()

NUM_TRIAL = 100
scores = []

for _ in range(NUM_TRIAL):
    game = Game()
    update = {'r': game.move_right,
              'l': game.move_left,
              'u': game.move_up,
              'd': game.move_down}
    game.put_tile()
    game.put_tile()
    while game.playable():
        direction = res.solve(game.board)
        if not update[direction]():
            continue
        game.put_tile()
    scores.append(game.score)

print("average score : {}".format(sum(scores) / NUM_TRIAL))
