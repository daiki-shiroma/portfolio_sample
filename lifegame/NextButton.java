package lifegame;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JButton;
import javax.swing.JPanel;
public class NextButton extends JPanel implements ActionListener {
		private BoardModel model;
		private BoardView  view;
		private JButton  undo;
		public NextButton(BoardModel model,BoardView view,JButton  undo){
		        this.model = model; // mainからmodelを受け取る
		        this.view  = view; //mainからviewを受け取る
		        this.undo  = undo;
		 }
		@Override
		public void actionPerformed(ActionEvent e) {
			System.out.println("NextButton was clicked.");
			 model.next();
			 view.repaint();	
			 undo.setEnabled(true);
		}		
}

