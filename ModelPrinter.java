package lifegame;
import javax.swing.JPanel;
public class ModelPrinter extends JPanel implements BoardListener {
	
	public void updated(BoardModel model) {
        model.printForDebug();
              
}	
}
