package lifegame;
import javax.swing.SwingUtilities;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JButton;
import java.awt.Dimension;
import java.awt.BorderLayout;
import java.awt.FlowLayout;

public class Main implements Runnable {
	
	public static void main(String[] args) {
		SwingUtilities.invokeLater(new Main());
	}
	public void run() {
		BoardModel model = new BoardModel(12, 12);
		model.addListener(new ModelPrinter()); // 新規に追加する行
       
       /* model.changeCellState(1, 1);
        model.changeCellState(2, 2);    
        model.changeCellState(0, 3);   
        model.changeCellState(1, 3);     
        model.changeCellState(2, 3);    
        model.changeCellState(4, 4);   
        System.out.println("init");
        model.changeCellState(4, 4);*/
    
        
        /*System.out.println("start");
        for (int i=0; i<12; ++i) {
            model.next();
          }
        System.out.println("finish");
        while (model.isUndorable()) {
            model.undo();
        }
        System.out.println("Restart");
        model.next();
        model.next();*/
        JFrame frame = new JFrame();
        frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        frame.setTitle("Lifegame");
        JPanel base = new JPanel();
        frame.setContentPane(base);
        base.setPreferredSize(new Dimension(400, 300));
        frame.setMinimumSize(new Dimension(300, 200));
        base.setLayout(new BorderLayout());
        
        
        
        BoardView view = new BoardView(model);
        base.add(view, BorderLayout.CENTER);
        
        
        
        JPanel buttonPanel = new JPanel();
        base.add(buttonPanel, BorderLayout.SOUTH);
        buttonPanel.setLayout(new FlowLayout());
        
       
        
        JButton Undo = new JButton("Undo");
        
        JButton Next = new JButton("Next");
        buttonPanel.add(Next);
        Next.addActionListener(new NextButton(model,view,Undo));
       
       
        buttonPanel.add(Undo);
        Undo.addActionListener( new UndoButton(model,view,Undo));
        Undo.setEnabled(false);
        
        
        
        JButton NewGame = new JButton("NewGame");
        buttonPanel.add(NewGame);
        NewGame.addActionListener(new NewGameButton());
        
       
        frame.pack();
        frame.setVisible(true); 
        
       
    } 
	
	
}