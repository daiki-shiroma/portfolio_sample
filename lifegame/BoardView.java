package lifegame;
import javax.swing.JPanel;
import java.awt.Graphics;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.util.ArrayList;

public class BoardView extends JPanel implements MouseListener, MouseMotionListener {
	private int printcols ;   
	private int printrows; 
	//private boolean[][] temp;
	BoardModel model;
//	private int x;
//	private int y;
	private ArrayList<Integer> listx;
	private ArrayList<Integer> listy;
	private ArrayList<Integer> initx;
	private ArrayList<Integer> inity;
	private ArrayList<Integer> listprex;
	private ArrayList<Integer> listprey;
	public BoardView  (BoardModel model) {
		printcols=model.cols;
		printrows=model.rows;
		this.model=model;	
		/*temp= new boolean [printcols+2][printrows+2];
		for (int i = 0; i <printrows+2; i++) {  //前状態をコピー
            for (int j = 0; j < printcols+2; j++) {
                temp[i][j] = mo.cells[i][j];
            }
        } */
		initx= new ArrayList<Integer>();
		inity= new ArrayList<Integer>();
		listx= new ArrayList<Integer>();
		listy= new ArrayList<Integer>();
		listprex= new ArrayList<Integer>();
		listprey= new ArrayList<Integer>();
		this.addMouseListener(this);
        this.addMouseMotionListener(this);
	}
	
	
	
	
	public boolean isAlive(int x, int y) {		
	    	if(model.cells[x][y]==true) return true;
	    	else return false;	
	    }
    @Override
    public void paint(Graphics g) {
    	
    	super.paint(g);
    	Graphics g2 = (Graphics) g.create();  
    	int w=this.getWidth();
    	int h=this.getHeight();
    	for (int i=0;i<=printcols+1;i++) {
    	    int b=h/(printrows);
    		b=b*i;
      		g2.drawLine(0, b, w, b);    		
      	}
    	for (int i=0;i<=printrows+1;i++ ) {
    		int a=w/(printcols);
    		a=a*i;
      	    g2.drawLine(a, 0, a, h); 
    	}
    	
  
    	for (int i = 1; i <printrows+1; i++) {  //前状態をコピー
            for (int j = 1; j < printcols+1; j++) {
            	int a=w/(printcols);
            	int b=h/(printrows);
            	a=a*(i-1);
            	b=b*(j-1);
                if(isAlive(i,j)) {g2.fillRect(a, b, w/(printcols), h/(printrows));}
            }
        }
    	g2.dispose();    	
    } 	
    
    
    
    public void mouseClicked(MouseEvent e) {}
    public void mouseEntered(MouseEvent e) {}
    public void mouseExited(MouseEvent e) {}
    public void mousePressed(MouseEvent e) {
    	int w=this.getWidth();
    	int h=this.getHeight();
    	int a=w/(printcols);
    	int b=h/(printrows);
    	int x=0;
        int y=0;
    	if(e.getX()%a!=0)x=e.getX()/a+1;
        else x=e.getX()/a;     
        if(e.getY()%b!=0)y=e.getY()/b+1;
        else y=e.getY()/b;
        initx.add(x);
        inity.add(y);
    	model.changeCellState(x-1, y-1);
    	this.repaint();   
    	if(initx.size()>1) initx.remove(0);
    	if(inity.size()>1) inity.remove(0);
 
    }
    
    public void mouseReleased(MouseEvent e) {}
    
    public void mouseDragged(MouseEvent e) {
    	
    	int w=this.getWidth();
    	int h=this.getHeight();
    	int a=w/(printcols);
    	int b=h/(printrows);       
        int x=0;
        int y=0;
        if(e.getX()%a!=0)x=e.getX()/a+1;
        else x=e.getX()/a;
        
        if(e.getY()%b!=0)y=e.getY()/b+1;
        else y=e.getY()/b;
      
    	if(listx.size()==0) listx.add(x);
    	if(listy.size()==0) listy.add(y);	
    	if(listprex.size()==0) listprex.add(-1);   //ダミー
    	if(listprey.size()==0) listprey.add(-1);
    	
    	if(x==listprex.get(0) &&y==listprey.get(0) &&listx.get(0)!=initx.get(0)) {
			model.changeCellState(listx.get(0)-1, y-1);
			System.out.println("NextButtonx was clicked.");
			this.repaint();
			}
    	
    	if(x!=listx.get(0)) {
    		if(x!=initx.get(0)||y!=inity.get(0))
    		{
    		model.changeCellState(x-1, y-1); 
    		this.repaint();
    		listprex.remove(0);
    		listprex.add(listx.get(0));
    		listprey.remove(0);
    		listprey.add(listy.get(0));
    		}
    	}
    	 listx.remove(0);
    	 listx.add(x);
    	
    	 if(x==listprex.get(0)&&y==listprey.get(0) && listy.get(0)!=inity.get(0)) {    	
 			model.changeCellState(x-1, listy.get(0)-1);
 			System.out.println("NextButtony was clicked.");
 			this.repaint();
 			}
    	
    	 if(y!=listy.get(0)) {
    		 if(x!=initx.get(0)||y!=inity.get(0)) {
    		 model.changeCellState(x-1, y-1); 
    		 this.repaint();	
    		 listprex.remove(0);
     		 listprex.add(listx.get(0));
    		 listprey.remove(0);
     		 listprey.add(listy.get(0));
    	 }
    	 }
    	 listy.remove(0);
    	 listy.add(y);
    	   
    }
    public void mouseMoved(MouseEvent e) {}
    
}
