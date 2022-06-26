package lifegame;
import java.util.ArrayList;
import javax.swing.JPanel;
public class BoardModel  extends JPanel {
	public int cols;   
	public int rows; 
    private ArrayList<BoardListener> listeners;
    public BoardModel(int c, int r) {
       cols = c;
       rows = r; 
       cells = new boolean[rows+2][cols+2];
       nextCells = new boolean[rows+2][cols+2];
       listeners = new ArrayList<BoardListener>();
       list= new ArrayList<boolean[][]>();
    }
    public int getCols() {
       return cols;
    }
    public int getRows() {
        return rows;
    }
    
    public boolean[][] cells; //private
    private boolean[][] nextCells;
    private boolean[][] temp;
    private ArrayList<boolean[][]> list;
    public void printForDebug() {
    	for (int j=1;j<=cols;j++){

    		  for (int i=1;i<=rows;i++){
    		   
    		    if(cells[i][j]==true) {System.out.print("*");}

    		    else {System.out.print(".");}
    		  }
    		   System.out.println();
    		} 
    	 System.out.println();
    }
    
    public void changeCellState(int x, int y) { 
    	// (x, y) で指定されたセルの状態を変更する。
    	if(cells[x+1][y+1]==true) cells[x+1][y+1]=false;
    	else cells[x+1][y+1]=true;
  
    	this.fireUpdate();
    	}
    
    public void addListener(BoardListener listener) {
    	listeners.add(listener);
    }

    public void fireUpdate() {
    	for(BoardListener listener: listeners) {
    		listener.updated(this);
    	}
    }
    
    
    public void next() {
    	
    	temp= new boolean [rows+2][cols+2];
    	for (int i = 0; i <rows+2; i++) {  //前状態をコピー
            for (int j = 0; j < cols+2; j++) {
                temp[i][j] = cells[i][j];
            }
        }
    	

        if(list.size()<32) 	list.add(temp);
    	
        else {
        	list.remove(0);
        	list.add(temp);
        }
        
        
        
    	for (int j=1;j<=cols;j++){    //ライフゲーム
    		
  		  for (int i=1;i<=rows;i++){
  			int count = 0;
            // 左上、真上、右上
            if(cells[i - 1][j - 1])count++;
            if(cells[i][j - 1])count++;
            if(cells[i + 1][j - 1])count++;
            
            // 左、右
            if(cells[i - 1][j])count++;
            if(cells[i + 1][j])count++;
            
            // 左下、真下、右下
            if(cells[i - 1][j + 1])count++;
            if(cells[i][j + 1])count++;
            if(cells[i + 1][j + 1])count++;

            // ライフゲームの規則により次世代の生死判定
            if (cells[i][j] && count <= 1)
            {
                nextCells[i][j] = false; // 過疎で死滅
            }

            else if (cells[i][j] && (count == 2 || count == 3))
            {
                nextCells[i][j] = true; // 生存
            }

            else if (cells[i][j]&& count >= 4)
            {
                nextCells[i][j] = false; // 過密で死滅
            }

            else if (!cells[i][j] && count == 3)
            {
                nextCells[i][j] = true; // 誕生
            }

            else
            {
                nextCells[i][j] = false;
            }  	    
  	       }		   
  		}
    	// 次世代の状態を反映
        for (int i = 0; i <rows+2; i++) {
            for (int j = 0; j < cols+2; j++) {
                cells[i][j] = nextCells[i][j];
            }
        }
        this.fireUpdate();
      }
    
   public void undo() {
	   int lastIdx = list.size() - 1;
	   if (lastIdx<0) {		   
		   return;
	   }
		   
	   
	   for (int i = 0; i <rows+2; i++) {
           for (int j = 0; j < cols+2; j++) {
               cells[i][j] = list.get(lastIdx)[i][j];
           }
       }
    	list.remove(lastIdx);
    	this.fireUpdate();
    	return;
    }
   
   public boolean isUndorable() {   
	   if (list.size()==0) return false;
	   return true;
    }  
}
    
