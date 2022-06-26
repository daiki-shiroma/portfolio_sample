package lifegame;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JPanel;
public class NewGameButton extends JPanel implements ActionListener {
	@Override
	public void actionPerformed(ActionEvent e) {
		System.out.println("NewGameButton  was clicked.");
		lifegame.Main p=new lifegame.Main();
		p.run();
	}

}
