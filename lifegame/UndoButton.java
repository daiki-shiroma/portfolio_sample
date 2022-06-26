package lifegame;
import javax.swing.JButton;
import javax.swing.JPanel;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
public class UndoButton extends JPanel implements ActionListener { 
	
	private BoardModel model;
	private BoardView  view;
	private JButton  undo;
	public UndoButton(BoardModel model,BoardView view, JButton  undo){
	        this.model = model;
	        this.view  = view;
	        this.undo  = undo;
	    }
	 public void actionPerformed(ActionEvent e){ 
		 System.out.println("UndoButton  was clicked.");	
		 model.undo();		
		 view.repaint();
		 if(!model.isUndorable()) undo.setEnabled(false);
	 } 
	 
}