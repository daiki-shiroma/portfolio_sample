import random
import math
from game import Game


class Solver():
    def __init__(self) -> None:
        pass

    def solve(self, board) -> str:
        
        d = ['r','u', 'd','l']
      
        best_move=-1
        count_fill=0  
        ans = []
        count_list = [] 
        max_list = [] 
        action = {'r': Game.move_right,
                  'u': Game.move_up,
                  'd': Game.move_down,
                  'l': Game.move_left}
      
        for x in range(4):
          for y in range(4):
            if(board[x][y] >= 2) :
                  count_fill+=1
        
        if (count_fill <=8):
          for i in range(4):
            game = Game(board)            
            is_movable= action[d[i]](game)
            maxvalue=0
            smooth=0
            count_leftspace=0
            raiser=0
            if  is_movable:             
              for x in range(4):
                for y in range(4):
                  if(game.board[x][y] >= 2) :           
                    if (maxvalue<=game.board[x][y]):
                      maxvalue = game.board[x][y]
                      
              for x in range(3):
               
                  if  game.board[x][3] > game.board[x+1][3] :
                      raiser+=1   

              if  game.board[3][3] > game.board[3][2] :
                      raiser+=1   

              for x in range(3):
               
                  if  game.board[2-x][2] > game.board[3-x][2] :
                      raiser+=1   


          
              for x in range(3):
                for y in range(4):
                  if game.board[x][y] >=16 and game.board[x][y] == game.board[x+1][y] :
                     smooth+=1
              
              for x in range(4):
               for y in range(0,2):
                 if game.board[x][y]==0 :
                     count_leftspace+=1
              
              max_list.append(int(math.log2(maxvalue)+3*raiser+2*smooth+3*count_leftspace))
            else:
              max_list.append(int(0))

         
          best_move = max_list.index(max(max_list))
             

        else :
          for i in range(3):
            game = Game(board)            
            is_movable= action[d[i]](game)
            smooth=0
            count=0
            count_leftspace=0
            if  is_movable:             
              for x in range(4):
                for y in range(4):
                  if(game.board[x][y] == 0) :
                    count+=1

              for x in range(0,3):
                for y in range(4):
                  if game.board[x][y] >=16 and game.board[x][y] == game.board[x+1][y] :        
                     smooth+=1       

              for x in range(4):
               for y in range(0,2):
                 if game.board[x][y]==0 :
                     count_leftspace+=1
              
               if  count_fill >=14:
                count*=100

              count_list.append(int(count+3*smooth+count_leftspace)) 
            else :
              count_list.append(int(0))

                       
          if count_list[0]==0 and count_list[1]==0 and count_list[2]==0 :
             best_move =3

          else:
             best_move = count_list.index(max(count_list))

        ans = d[best_move]        
        return ans


if __name__ == '__main__':
    game = Game([[2, 2, 0, 0], [0, 4, 0, 4], [0, 0, 0, 4], [4, 0, 4, 4]])
    sol = Solver()
    print(sol.solve(game.board)) 