/*
 * This file is part of "The Java Telnet Application".
 *
 * (c) Matthias L. Jugel, Marcus Mei�ner 1996-2002. All Rights Reserved.
 *
 * Please visit http://javatelnet.org/ for updates and contact.
 *
 * --LICENSE NOTICE--
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * --LICENSE NOTICE--
 *
 */

package de.mud.terminal;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import de.mud.jta.plugin.Terminal;


/**
 * Video Display Unit emulation for Swing/AWT. This class implements all necessary
 * features of a character display unit, but not the actual terminal emulation.
 * It can be used as the base for terminal emulations of any kind.
 * <P>
 * This is a lightweight component. It will render very badly if used
 * in standard AWT components without overloaded update() method. The
 * update() method must call paint() immediately without clearing the
 * components graphics context or parts of the screen will simply
 * disappear.
 * <P>
 * <B>Maintainer:</B> Matthias L. Jugel
 *
 * @version $Id: SwingTerminal.java,v 1.4 2004/07/16 18:04:44 jennifer Exp $
 * @author  Matthias L. Jugel, Marcus Mei�ner
 */
public class SwingTerminal extends Component
        implements VDUDisplay, KeyListener, ActionListener, MouseListener, MouseMotionListener {

  private final static int debug = 0;
 
  private Terminal terminal;

  /** the VDU buffer */
  private VDUBuffer buffer;

  /** lightweight component definitions */
  private final static long VDU_EVENTS = AWTEvent.KEY_EVENT_MASK
          | AWTEvent.FOCUS_EVENT_MASK
          | AWTEvent.MOUSE_MOTION_EVENT_MASK
          | AWTEvent.MOUSE_EVENT_MASK;

  private Insets insets;                            /* size of the border */
  private boolean raised;            /* indicator if the border is raised */

  private Font normalFont;                                 /* normal font */
  private FontMetrics fm;                         /* current font metrics */
  private int charWidth;                       /* current width of a char */
  private int charHeight;                     /* current height of a char */
  private int charDescent;                           /* base line descent */
  private int resizeStrategy;                /* current resizing strategy */

  private Point selectBegin, selectEnd;          /* selection coordinates */
  private String selection;                 /* contains the selected text */

  private JScrollBar scrollBar;
  private SoftFont sf = new SoftFont();

  private boolean colorPrinting = false;	/* print display in color */

  private Image backingStore = null;
  private Clipboard clipboard;


  /**
   * Create a color representation that is brighter than the standard
   * color but not what we would like to use for bold characters.
   * @param clr the standard color
   * @return the new brighter color
   */
  private Color brighten(Color clr) {
    int r,g,b;

    r = (int) min(clr.getRed() * 1.2, 255.0);
    g = (int) min(clr.getGreen() * 1.2, 255.0);
    b = (int) min(clr.getBlue() * 1.2, 255.0);
    return new Color(r, g, b);
  }

  /**
   * Create a color representation that is darker than the standard
   * color but not what we would like to use for bold characters.
   * @param clr the standard color
   * @return the new darker color
   */
  private Color darken(Color clr) {
    int r,g,b;

    r = (int) max(clr.getRed() * 0.8, 0.0);
    g = (int) max(clr.getGreen() * 0.8, 0.0);
    b = (int) max(clr.getBlue() * 0.8, 0.0);
    return new Color(r, g, b);
  }

  /** A list of colors used for representation of the display */
  private Color color[] = {Color.black,
                           Color.red,
                           Color.green,
                           Color.yellow,
                           Color.blue,
                           Color.magenta,
                           Color.cyan,
                           Color.white,
                           null, // bold color
                           null, // inverted color
  };

  public final static int RESIZE_NONE = 0;
  public final static int RESIZE_FONT = 1;
  public final static int RESIZE_SCREEN = 2;
  public final static int COLOR_BOLD = 8;
  public final static int COLOR_INVERT = 9;


  /* definitions of standards for the display unit */
  private final static int COLOR_FG_STD = 7;
  private final static int COLOR_BG_STD = 0;

  /** User defineable cursor colors */
  private Color cursorColorFG = null;
  private Color cursorColorBG = null;

  private JPopupMenu popup;
  private MouseAdapter adapter;
  private Component selectedComponent;

  protected double max(double f1, double f2) {
    return (f1 < f2)?f2:f1;
  }

  protected double min(double f1, double f2) {
    return (f1 < f2)?f1:f2;
  }

  //popups are in this terminal as this is the component
  //maybe move back up to terminal ????
  //REMIND. later. maybe this doesnt belong here according to jta plugin design
  public void addPopups() {
	popup = new JPopupMenu();

	//PRINT not used yet
   //popup.add( makeMenuItem("print") );

   popup.add( makeMenuItem("copy") );
   popup.add( makeMenuItem("paste") );
   popup.add( makeMenuItem("break") );
   popup.add( makeMenuItem("disconnect") );

	// Create a MouseAdapter that creates a Popup menu
   // when the right mouse or equivalent button clicked.
    adapter = new MouseAdapter()
    {
      // On some machines, mouseReleased sets
      // PopupTrigger.
      public void mouseReleased(MouseEvent e)
      {
        if ( e.isPopupTrigger() )
        {
			System.out.println("adapter mouse released");
          showPopupMenu(e);
        }
      }
      // But on other machines, mousePressed sets
      // PopupTrigger.
      public void mousePressed(MouseEvent e)
      {
        if ( e.isPopupTrigger() )
      {
			System.out.println("adapter mouse pressed");
        showPopupMenu(e);
      }
    }
    public void showPopupMenu(MouseEvent e)
    {
      selectedComponent = e.getComponent();
      // Get the component on which the button click
      // occurred and show the menu there.
      popup.show( selectedComponent, e.getX(), e.getY());
    }
   };
	getClipboard();
  }

	private JMenuItem makeMenuItem(String label)
	{
		JMenuItem item = new JMenuItem(label);
		item.setActionCommand(label);
    	item.addActionListener( this );
		return item;
	}
  public void actionPerformed(ActionEvent evt) {
	String command = evt.getActionCommand();
	//System.out.println("commnad = " + command);
	if (command.equals("print")) {
		//System.out.println("print");
        	print(); 
	} else if (command.equals("copy")) {
		//System.out.println("copy");
		copy(clipboard);
	} else if (command.equals("paste")) {
		//System.out.println("paste");
		paste(clipboard);
	} else if (command.equals("break")) {
		//System.out.println("break");
		terminal.breakn();
	} else if (command.equals("disconnect")) {
		//System.out.println("disconnect");
		terminal.disconnect();
	} 
  }

  public void copy(Clipboard clipboard) {
	terminal.copy(clipboard);
  }

  public void paste(Clipboard clipboard) {
	terminal.paste(clipboard);
  }

  public void print() {
	terminal.print();
  }

   public void getClipboard() {
    // set up the clipboard
    //System.out.println("cut and paste...");
    try {
            clipboard = this.getToolkit().getSystemClipboard();
            //System.out.println("clipboard setup clipboard = " + clipboard);
            System.err.println("Applet: acquired system clipboard: " + clipboard);
    } catch (Exception e) {
            System.err.println("Applet: system clipboard access denied: " + ((e instanceof InvocationTargetException) ?  ((InvocationTargetException) e).getTargetException() : e));
            // e.printStackTrace();
    } finally {
            if (clipboard == null) {
            	//System.out.println("clipboard null");
            	System.err.println("Applet: copy & paste only within the JTA");
            	clipboard = new Clipboard("de.mud.jta.Main");
            }
    }
  }



  /**
   * Create a new video display unit with the passed width and height in
   * characters using a special font and font size. These features can
   * be set independently using the appropriate properties.
   * @param buffer a VDU buffer to be associated with the display
   * @param font the font to be used (usually Monospaced)
   */
  public SwingTerminal(VDUBuffer buffer, Font font) {
    addPopups();
    setVDUBuffer(buffer);
    //System.out.println("windowBase = " + buffer.getWindowBase());
    addKeyListener(this);
    //addActionListener(this);


    /* we have to make sure the tab key stays within the component */
    String version = System.getProperty("java.version");
    if (version.startsWith("1.4")) {
      try {
        Class params[] = new Class[]{boolean.class};
        SwingTerminal.class.getMethod("setFocusable", params).invoke(this, new Object[]{new Boolean(true)});
        SwingTerminal.class.getMethod("setFocusTraversalKeysEnabled", params).invoke(this, new Object[]{new Boolean(false)});
      } catch (Exception e) {
        System.err.println("vt320: unable to reset focus handling for java version " + version);
        e.printStackTrace();
      }
    }

    // lightweight component handling
    enableEvents(VDU_EVENTS);

    // set the normal font to use
    setFont(font);
    // set the standard resize strategy
    setResizeStrategy(RESIZE_FONT);

    setForeground(Color.white);
    setBackground(Color.black);

    cursorColorFG = color[COLOR_FG_STD];
    cursorColorBG = color[COLOR_BG_STD];

    clearSelection();

    addMouseListener(this);
    addMouseListener(adapter);
    addMouseMotionListener(this);
    addActionListener(this);

    setBorder(2, false);
    selection = null;
  }

  /**
   * Create a display unit with size 80x24 and Font "Monospaced", size 12.
   */
  public SwingTerminal(Terminal terminal, VDUBuffer buffer) {
    this(buffer, new Font("Monospaced", Font.PLAIN, 11));
    this.terminal = terminal;
  }
  
  public SwingTerminal(VDUBuffer buffer) {
    this(buffer, new Font("Monospaced", Font.PLAIN, 11));
  }

  /**
   * Set a new terminal (VDU) buffer.
   * @param buffer new buffer
   */
  public void setVDUBuffer(VDUBuffer buffer) {
    this.buffer = buffer;
    buffer.setDisplay(this);
  }

  /**
   * Return the currently associated VDUBuffer.
   * @return the current buffer
   */
  public VDUBuffer getVDUBuffer() {
    return buffer;
  }

  /**
   * Set new color set for the display.
   * @param colorset new color set
   */
  public void setColorSet(Color[] colorset) {
    System.arraycopy(colorset, 0, color, 0, 10);
    buffer.update[0] = true;
    redraw();
  }

  /**
   * Get current color set.
   * @return the color set currently associated
   */
  public Color[] getColorSet() {
    return color;
  }

  /**
   * Set the font to be used for rendering the characters on screen.
   * @param font the new font to be used.
   */
  public void setFont(Font font) {
    super.setFont(normalFont = font);
    fm = getFontMetrics(font);
    if (fm != null) {
      charWidth = fm.charWidth('@');
      charHeight = fm.getHeight();
      charDescent = fm.getDescent();
    }
    if (buffer.update != null) buffer.update[0] = true;
    redraw();
  }

  /**
   * Set the strategy when window is resized.
   * RESIZE_FONT is default.
   * @param strategy the strategy
   * @see #RESIZE_NONE
   * @see #RESIZE_FONT
   * @see #RESIZE_SCREEN
   */
  public void setResizeStrategy(int strategy) {
    resizeStrategy = strategy;
  }

  /**
   * Set the border thickness and the border type.
   * @param thickness border thickness in pixels, zero means no border
   * @param raised a boolean indicating a raised or embossed border
   */
  public void setBorder(int thickness, boolean raised) {
    if (thickness == 0)
      insets = null;
    else
      insets = new Insets(thickness + 1, thickness + 1,
                          thickness + 1, thickness + 1);
    this.raised = raised;
  }

  /**
   * Connect a scrollbar to the VDU. This should be done differently
   * using a property change listener.
   * @param scrollBar the scroll bar
   */
  public void setScrollbar(JScrollBar scrollBar) {
    if (scrollBar == null) return;
    this.scrollBar = scrollBar;
    this.scrollBar.setValues(buffer.windowBase, buffer.height, 0, buffer.maxBufSize);
    this.scrollBar.addAdjustmentListener(new AdjustmentListener() {
      public void adjustmentValueChanged(AdjustmentEvent evt) {
        if (buffer.getWindowBase()!=evt.getValue()) {
            buffer.setWindowBase(evt.getValue());
						if (debug > 0) 
							System.out.println("new windowBase value:"+evt.getValue());
        }
      }
    });

    buffer.addBufferListener(new BufferListener() {
			public void handleBufferEvent(BufferEvent e) {
				if (e.getType()==e.WINDOW_BASE_CHANGED) {
					SwingTerminal.this.scrollBar.setValues(buffer.windowBase, buffer.height, 0, buffer.height+buffer.screenBase);
					if (debug > 0)
          	System.out.println("new scrollbar value:"+e.getValue());
				} else if (e.getType()==e.SCREEN_SIZE_CHANGED) {
					SwingTerminal.this.scrollBar.setValues(buffer.windowBase, buffer.height, 0, buffer.height+buffer.screenBase);
					if (debug > 0)
						System.out.println("Setting screen, base:"+buffer.windowBase+" height:"+buffer.height+" maxBufSize:"+buffer.maxBufSize);
				} else if (e.getType()==e.BUFFER_SIZE_CHANGED) {
					SwingTerminal.this.scrollBar.setValues(buffer.windowBase, buffer.height, 0, buffer.height+buffer.screenBase);
					if (debug > 0) 
						System.out.println("Buffer size changed");
				}
			}
    });
  }

  /**
   * Redraw marked lines.
   */
  public void redraw() {
    if (backingStore != null) {
      redraw(backingStore.getGraphics());
      repaint();
    }
  }

  protected void redraw(Graphics g) {
    //System.out.println("redraw()");

    int xoffset = (super.getSize().width - buffer.width * charWidth) / 2;
    int yoffset = (super.getSize().height - buffer.height * charHeight) / 2;

    int selectStartLine = selectBegin.y - buffer.windowBase;
    int selectEndLine = selectEnd.y - buffer.windowBase;

    Color fg = darken(color[COLOR_FG_STD]);
    Color bg = darken(color[COLOR_BG_STD]);

    g.setFont(normalFont);


    /* for debug only
     if (update[0]) {
         System.err.println("Redrawing all");
     } else {
     for (int l = 1; l < size.height+1; l++) {
         if (update[l]) {
             for (int c = 0; c < size.height-l;c++) {
             if (!update[c+l]) {
             System.err.println("Redrawing "+(l-1)+" - "+(l+c-2));
             l=l+c;
             break;
             }
         }
         }
     }
     }
     */

    for (int l = 0; l < buffer.height; l++) {
      if (!buffer.update[0] && !buffer.update[l + 1]) continue;
      buffer.update[l + 1] = false;
      //if (debug > 2) System.err.println("redraw(): line " + l);
      //System.out.println("redraw(): buffer.width= " + buffer.width);
      for (int c = 0; c < buffer.width; c++) {
        int addr = 0;
        int currAttr = buffer.charAttributes[buffer.windowBase + l][c];

        fg = darken(getForeground());
        bg = darken(getBackground());

        if ((currAttr & buffer.COLOR_FG) != 0)
          fg = darken(color[((currAttr & buffer.COLOR_FG) >> 4) - 1]);
        if ((currAttr & buffer.COLOR_BG) != 0)
          bg = darken(darken(color[((currAttr & buffer.COLOR_BG) >> 8) - 1]));

        if ((currAttr & VDUBuffer.BOLD) != 0) {
          g.setFont(new Font(normalFont.getName(), Font.BOLD, normalFont.getSize()));
          // does not work with IE6: g.setFont(normalFont.deriveFont(Font.BOLD));
          if (null != color[COLOR_BOLD]) {
            fg = color[COLOR_BOLD];
          }
          /*
          if(fg.equals(Color.black)) {
            fg = Color.gray;
          } else {
            fg = brighten(fg);
            // bg = bg.brighter(); -- make some programs ugly
          }
          */
        } else {
          g.setFont(normalFont);
        }

        if ((currAttr & VDUBuffer.LOW) != 0) {
          fg = darken(fg);
        }
        if ((currAttr & VDUBuffer.INVERT) != 0) {
          if (null == color[COLOR_INVERT]) {
            Color swapc = bg;
            bg = fg;
            fg = swapc;
          } else {
            if (null == color[COLOR_BOLD]) {
              fg = bg;
            } else {
              fg = color[COLOR_BOLD];
            }
            bg = color[COLOR_INVERT];
          }
        }

        if (sf.inSoftFont(buffer.charArray[buffer.windowBase + l][c])) {
          g.setColor(bg);
          g.fillRect(c * charWidth + xoffset, l * charHeight + yoffset,
                     charWidth, charHeight);
          g.setColor(fg);
          sf.drawChar(g, buffer.charArray[buffer.windowBase + l][c], xoffset + c * charWidth,
                      l * charHeight + yoffset, charWidth, charHeight);
          if ((currAttr & VDUBuffer.UNDERLINE) != 0)
            g.drawLine(c * charWidth + xoffset,
                       (l + 1) * charHeight - charDescent / 2 + yoffset,
                       c * charWidth + charWidth + xoffset,
                       (l + 1) * charHeight - charDescent / 2 + yoffset);
          continue;
        }

        // determine the maximum of characters we can print in one go
        while ((c + addr < buffer.width) &&
                ((buffer.charArray[buffer.windowBase + l][c + addr] < ' ') ||
                (buffer.charAttributes[buffer.windowBase + l][c + addr] == currAttr)) &&
                !sf.inSoftFont(buffer.charArray[buffer.windowBase + l][c + addr])) {
          if (buffer.charArray[buffer.windowBase + l][c + addr] < ' ') {
            buffer.charArray[buffer.windowBase + l][c + addr] = ' ';
            buffer.charAttributes[buffer.windowBase + l][c + addr] = 0;
            continue;
          }
          addr++;
        }

        // clear the part of the screen we want to change (fill rectangle)
        g.setColor(bg);
        g.fillRect(c * charWidth + xoffset, l * charHeight + yoffset,
                   addr * charWidth, charHeight);

        g.setColor(fg);

        // draw the characters
	//System.out.println("drawing characters....from winbase = " + (buffer.windowBase + 1));
	//System.out.println("c = " + c + " addr= " + addr + " yoffset= " + yoffset);
	//System.out.println("xoffset = " + xoffset + " l+1= " + (l+1));
        g.drawChars(buffer.charArray[buffer.windowBase + l], c, addr,
                    c * charWidth + xoffset,
                    (l + 1) * charHeight - charDescent + yoffset);

        if ((currAttr & VDUBuffer.UNDERLINE) != 0)
          g.drawLine(c * charWidth + xoffset,
                     (l + 1) * charHeight - charDescent / 2 + yoffset,
                     c * charWidth + addr * charWidth + xoffset,
                     (l + 1) * charHeight - charDescent / 2 + yoffset);

        c += addr - 1;
      }

      // selection code, highlites line or part of it when it was
      // selected previously
      if (l >= selectStartLine && l <= selectEndLine) {
        int selectStartColumn = (l == selectStartLine ? selectBegin.x : 0);
        int selectEndColumn =
                (l == selectEndLine ?
                (l == selectStartLine ? selectEnd.x - selectStartColumn :
                selectEnd.x) : buffer.width);
        if (selectStartColumn != selectEndColumn) {
          if (debug > 0)
            System.err.println("**v3 select(" + selectStartColumn + "-"
                               + selectEndColumn + ")");
	
	  //FIX for selectEndColumn 
	  if ((selectStartColumn < 0)) { //&& (l != selectStartLine)) {
		selectEndColumn = selectEndColumn + selectStartColumn;
		selectStartColumn = 0;
 	  } 


	  if (((selectEndColumn + selectStartColumn) > buffer.width)) {
		selectEndColumn = buffer.width - selectStartColumn;
	  } 
         
          //if (selectStartColumn < 0) selectStartColumn = 0; 

          //FIX for painting just the selection, not past width
	  

	  //System.out.println("calcd selectEndColumn = " + selectEndColumn);
          g.setXORMode(bg);
          g.fillRect(selectStartColumn * charWidth + xoffset,
                     	l * charHeight + yoffset,
                     	selectEndColumn * charWidth,
                     	charHeight);
          g.setPaintMode();
        }
      }
    }

    // draw cursor
    if (buffer.showcursor && (
            buffer.screenBase + buffer.cursorY >= buffer.windowBase &&
            buffer.screenBase + buffer.cursorY < buffer.windowBase + buffer.height)
    ) {
      g.setColor(cursorColorFG);
      g.setXORMode(cursorColorBG);
      g.fillRect(buffer.cursorX * charWidth + xoffset,
                 (buffer.cursorY + buffer.screenBase - buffer.windowBase) * charHeight + yoffset, charWidth, charHeight);
      g.setPaintMode();
      g.setColor(color[COLOR_FG_STD]);
    }

    // draw border
    if (insets != null) {
      g.setColor(raised ? Color.LIGHT_GRAY : Color.BLACK);
      xoffset--;
      yoffset--;
      for (int i = insets.top - 1; i >= 0; i--)
      {
      	if (i == 0)
      		g.setColor(Color.LIGHT_GRAY);
        g.drawRect(xoffset - i, yoffset - i,
                     charWidth * buffer.width + 1 + i * 2,
                     charHeight * buffer.height + 1 + i * 2);
      }
    }
    buffer.update[0] = false;
  }

  /**
   * Paint the current screen using the backing store image.
   */
  public void paint(Graphics g) {
    if (backingStore == null) {
      Dimension size = super.getSize();
      backingStore = createImage(size.width, size.height);
      buffer.update[0] = true;
      redraw();
    }

    if (debug > 1)
      System.err.println("Clip region: " + g.getClipBounds());

    g.drawImage(backingStore, 0, 0, this);
  }

/*
  public int print(Graphics g, PageFormat pf, int pi) throws PrinterException {
    if(pi >= 1) {
      return Printable.NO_SUCH_PAGE;
    }
    paint(g);
    return Printable.PAGE_EXISTS;
  }
*/

  /**
   * Set default for printing black&amp;white or colorized as displayed on
   * screen.
   * @param colorPrint true = print in full color, default b&amp;w only
   */
  public void setColorPrinting(boolean colorPrint) {
    colorPrinting = colorPrint;
  }

  public void print(Graphics g) {
    if (debug > 0) System.err.println("DEBUG: print()");
    for (int i = 0; i <= buffer.height; i++) buffer.update[i] = true;
    Color fg = null, bg = null, colorSave[] = null;
    if (!colorPrinting) {
      fg = getForeground();
      bg = getBackground();
      setForeground(Color.black);
      setBackground(Color.white);
      colorSave = color;
      color = new Color[]{Color.black,
                          Color.black,
                          Color.black,
                          Color.black,
                          Color.black,
                          Color.black,
                          Color.black,
                          Color.white,
                          null,
                          null,
      };
    }

    redraw(g);

    if (!colorPrinting) {
      color = colorSave;
      setForeground(fg);
      setBackground(bg);
    }
  }

  /**
   * Convert Mouse Event coordinates into character cell coordinates
   * @param  evtpt the mouse point to be converted
   * @return Character cell coordinate of passed point
   */
  public Point mouseGetPos(Point evtpt) {
    Point mousepos;

    mousepos = new Point(0, 0);

    int xoffset = (super.getSize().width - buffer.width * charWidth) / 2;
    int yoffset = (super.getSize().height - buffer.height * charHeight) / 2;

    mousepos.x = (evtpt.x - xoffset) / charWidth;
    if (mousepos.x < 0) mousepos.x = 0;
    if (mousepos.x >= buffer.width) mousepos.x = buffer.width - 1;

    mousepos.y = (evtpt.y - yoffset) / charHeight;
    if (mousepos.y < 0) mousepos.y = 0;
    if (mousepos.y >= buffer.height) mousepos.y = buffer.height - 1;

    return mousepos;
  }

  /**
   * Set cursor FG and BG colors
   * @param fg foreground color or null
   * @param bg background color or null
   */
  public void setCursorColors(Color fg, Color bg) {
    if (fg == null)
      cursorColorFG = color[COLOR_FG_STD];
    else
      cursorColorFG = fg;
    if (bg == null)
      cursorColorBG = color[COLOR_BG_STD];
    else
      cursorColorBG = bg;
    repaint();
  }

  /**
   * Reshape character display according to resize strategy.
   * @see #setResizeStrategy
   */
  public void setBounds(int x, int y, int w, int h) {
    if (debug > 0)
      System.err.println("VDU: setBounds(" + x + "," + y + "," + w + "," + h + ")");

    super.setBounds(x, y, w, h);

    // ignore zero bounds
    if (x == 00 && y == 0 && w == 0 && h == 0) {
      return;
    }

    if (insets != null) {
      w -= insets.left + insets.right;
      h -= insets.top + insets.bottom;
    }

    if (debug > 0)
      System.err.println("VDU: looking for better match for " + normalFont);

    Font tmpFont = normalFont;
    String fontName = tmpFont.getName();
    int fontStyle = tmpFont.getStyle();
    fm = getFontMetrics(normalFont);
    if (fm != null) {
      charWidth = fm.charWidth('@');
      charHeight = fm.getHeight();
    }

    switch (resizeStrategy) {
      case RESIZE_SCREEN:
        buffer.setScreenSize(w / charWidth, buffer.height = h / charHeight);
        break;
      case RESIZE_FONT:
        int height = h / buffer.height;
        int width = w / buffer.width;

        fm = getFontMetrics(normalFont = new Font(fontName, fontStyle,
                                                  charHeight));

        // adapt current font size (from small up to best fit)
        if (fm.getHeight() < height || fm.charWidth('@') < width)
          do {
            fm = getFontMetrics(normalFont = new Font(fontName, fontStyle,
                                                      ++charHeight));
          } while (fm.getHeight() < height || fm.charWidth('@') < width);

        // now check if we got a font that is too large
        if (fm.getHeight() > height || fm.charWidth('@') > width)
          do {
            fm = getFontMetrics(normalFont = new Font(fontName, fontStyle,
                                                      --charHeight));
          } while (charHeight > 1 &&
                  (fm.getHeight() > height ||
                  fm.charWidth('@') > width));

        if (charHeight <= 1) {
          System.err.println("VDU: error during resize, resetting");
          normalFont = tmpFont;
          System.err.println("VDU: disabling font/screen resize");
          resizeStrategy = RESIZE_NONE;
        }

        setFont(normalFont);
        fm = getFontMetrics(normalFont);
        charWidth = fm.charWidth('@');
        charHeight = fm.getHeight();
        charDescent = fm.getDescent();
        break;
      case RESIZE_NONE:
      default:
        break;
    }
    if (debug > 0) {
      System.err.println("VDU: charWidth=" + charWidth + ", " +
                         "charHeight=" + charHeight + ", " +
                         "charDescent=" + charDescent);
    }

    // delete the double buffer image and mark all lines
    backingStore = null;
    buffer.markLine(0, buffer.height);
  }

  /**
   * Return the real size in points of the character display.
   * @return Dimension the dimension of the display
   * @see java.awt.Dimension
   */
  public Dimension getSize() {
    int xborder = 0, yborder = 0;
    if (insets != null) {
      xborder = insets.left + insets.right;
      yborder = insets.top + insets.bottom;
    }
    return new Dimension(buffer.width * charWidth + xborder,
                         buffer.height * charHeight + yborder);
  }

  /**
   * Return the preferred Size of the character display.
   * This turns out to be the actual size.
   * @return Dimension dimension of the display
   * @see #size
   */
  public Dimension getPreferredSize() {
    return getSize();
  }

  /**
   * The insets of the character display define the border.
   * @return Insets border thickness in pixels
   */
  public Insets getInsets() {
    return insets;
  }

  public void clearSelection() {
    selectBegin = new Point(0, 0);
    selectEnd = new Point(0, 0);
    selection = null;
  }

  public String getSelection() {
    return selection;
  }

  private boolean buttonCheck(int modifiers, int mask) {
    return (modifiers & mask) == mask;
  }

  public void mouseMoved(MouseEvent evt) {
    /* nothing yet we do here */
  }

  public void mouseDragged(MouseEvent evt) {
    //System.out.println("mouseDragged evt");
    if (buttonCheck(evt.getModifiers(), MouseEvent.BUTTON1_MASK) ||
            // Windows NT/95 etc: returns 0, which is a bug
            evt.getModifiers() == 0) {
      int xoffset = (super.getSize().width - buffer.width * charWidth) / 2;
      int yoffset = (super.getSize().height - buffer.height * charHeight) / 2;
      int x = (evt.getX() - xoffset) / charWidth;
      int y = (evt.getY() - yoffset) / charHeight + buffer.windowBase;
      int oldx = selectEnd.x, oldy = selectEnd.y;

      //System.out.println("mouseDragged (x,y)= " + "(" + x + "," + y + ")");
      if ((x <= selectBegin.x) && (y <= selectBegin.y)) {
        selectBegin.x = x;
        selectBegin.y = y;
      //Fix for changing direction 
      //needs to go in here
      } else { 
        selectEnd.x = x;
        selectEnd.y = y;
      }

      //System.out.println("selectBegin.y= " + selectBegin.y);
      //System.out.println("selectEnd.y= " + selectEnd.y);
      //REMIND> check this out tomorrow, wednesday
      //System.out.println("mouseDragged (selectBegin.x,selectBegin.y)= " + "(" + selectBegin.x + "," + selectBegin.y + ")");
      //System.out.println("mouseDragged (selectEnd.x,selectEnd.y)= " + "(" + selectEnd.x + "," + selectEnd.y + ")");

      //this bit works. md1
      if (oldx != x || oldy != y) {
        buffer.update[0] = true;
        if (debug > 0)
        System.err.println("select([" + selectBegin.x + "," + selectBegin.y + "]," +
                             "[" + selectEnd.x + "," + selectEnd.y + "])");
        redraw();
      }
      Dimension d = getSize();

      //mehul added. allows scrollbar to work if select past current page
      if (evt.getY() > (d.height + yoffset)) {
	int v = scrollBar.getValue();
	scrollBar.setValue(v + 1);
	//System.out.println("scrollbar set value + 1");
      } else if (evt.getY() < yoffset) {
	int v = scrollBar.getValue();
	if (scrollBar.getValue() >= 1) {
		scrollBar.setValue(v - 1);
		//System.out.println("scrollbar set value - 1");
	}
      }

    }
  }

  public void mouseClicked(MouseEvent evt) {
    /* nothing yet we do here */
  }

  public void mouseEntered(MouseEvent evt) {
    /* nothing yet we do here */
  }

  public void mouseExited(MouseEvent evt) {
    /* nothing yet we do here */
  }

  /**
   * Handle mouse pressed events for copy & paste.
   * @param evt the event that occured
   * @see java.awt.event.MouseEvent
   */
  public void mousePressed(MouseEvent evt) {
    //System.out.println("mousePressed evt");
    requestFocus();

    int xoffset = (super.getSize().width - buffer.width * charWidth) / 2;
    int yoffset = (super.getSize().height - buffer.height * charHeight) / 2;

    if (buffer instanceof VDUInput) {
      ((VDUInput) buffer).mousePressed(xoffset, yoffset, evt.getModifiers());
    }

    // looks like we get no modifiers here ... ... We do? -Marcus
    if (buttonCheck(evt.getModifiers(), MouseEvent.BUTTON1_MASK)) {
      selectBegin.x = (evt.getX() - xoffset) / charWidth;
      selectBegin.y = (evt.getY() - yoffset) / charHeight + buffer.windowBase;
      selectEnd.x = selectBegin.x;
      selectEnd.y = selectBegin.y;
      //System.out.println("selectBegin.y= " + selectBegin.y);
      //System.out.println("selectEnd.y= " + selectEnd.y);
    }

  }

  /**
   * Handle mouse released events for copy & paste.
   * @param evt the mouse event
   */
  public void mouseReleased(MouseEvent evt) {
    int xoffset = (super.getSize().width - buffer.width * charWidth) / 2;
    int yoffset = (super.getSize().height - buffer.height * charHeight) / 2;

    if (buffer instanceof VDUInput) {
      ((VDUInput) buffer).mousePressed(xoffset, yoffset, evt.getModifiers());
    }

    if (buttonCheck(evt.getModifiers(), MouseEvent.BUTTON1_MASK)) {
      mouseDragged(evt);

      if (selectBegin.x == selectEnd.x && selectBegin.y == selectEnd.y) {
        buffer.update[0] = true;
        redraw();
        return;
      }
      selection = "";
      // fix end.x and end.y, they can get over the border
      if (selectEnd.x < 0) selectEnd.x = 0;
      if (selectEnd.y < 0) selectEnd.y = 0;
      if (selectEnd.y >= buffer.charArray.length)
        selectEnd.y = buffer.charArray.length - 1;
      if (selectEnd.x > buffer.charArray[0].length)
        selectEnd.x = buffer.charArray[0].length;

	  if (selectBegin.y < 0) selectBegin.y = 0;
      	  for (int l = selectBegin.y; l <= selectEnd.y; l++) {
        	int start, end;
        	start = (l == selectBegin.y ? start = selectBegin.x : 0);
        	end = (l == selectEnd.y ? end = selectEnd.x : buffer.charArray[l].length);
        	// Trim all spaces from end of line, like xterm does.
		//System.out.println("start= " + start + " end= " + end + " l= " + l);
		if (start < 0) start = 0;
		if (end > buffer.width) end = buffer.width;
        	selection += ("-" + (new String(buffer.charArray[l])).substring(start, end)).trim().substring(1);
        	if (end == buffer.charArray[l].length)
          	selection += "\n";
      	  }
      
    }
  }

  public void keyTyped(KeyEvent e) {
    if (buffer != null)
      ((VDUInput) buffer).keyTyped(e.getKeyCode(), e.getKeyChar(), getModifiers(e));
  }

  public void keyPressed(KeyEvent e) {
    if (buffer != null)
      ((VDUInput) buffer).keyPressed(e.getKeyCode(), e.getKeyChar(), getModifiers(e));
  }

  public void keyReleased(KeyEvent e) {
    // ignore
  }

  // lightweight component event handling

  private MouseListener mouseListener;

  /**
   * Add a mouse listener to the VDU. This is the implementation for
   * the lightweight event handling.
   * @param listener the new mouse listener
   */
  public void addMouseListener(MouseListener listener) {
    mouseListener = AWTEventMulticaster.add(mouseListener, listener);
    enableEvents(AWTEvent.MOUSE_EVENT_MASK);
  }

  /**
   * Remove a mouse listener to the VDU. This is the implementation for
   * the lightweight event handling.
   * @param listener the mouse listener to remove
   */
  public void removeMouseListener(MouseListener listener) {
    mouseListener = AWTEventMulticaster.remove(mouseListener, listener);
  }

  private MouseMotionListener mouseMotionListener;

  /**
   * Add a mouse motion listener to the VDU. This is the implementation for
   * the lightweight event handling.
   * @param listener the mouse motion listener
   */
  public void addMouseMotionListener(MouseMotionListener listener) {
    mouseMotionListener = AWTEventMulticaster.add(mouseMotionListener, listener);
    enableEvents(AWTEvent.MOUSE_EVENT_MASK);
  }

  /**
   * Remove a mouse motion listener to the VDU. This is the implementation for
   * the lightweight event handling.
   * @param listener the mouse motion listener to remove
   */
  public void removeMouseMotionListener(MouseMotionListener listener) {
    mouseMotionListener =
            AWTEventMulticaster.remove(mouseMotionListener, listener);
  }

  /**
   * Process mouse events for this component. It will call the
   * methods (mouseClicked() etc) in the added mouse listeners.
   * @param evt the dispatched mouse event
   */
  public void processMouseEvent(MouseEvent evt) {
    // handle simple mouse events
    if (mouseListener != null)
      switch (evt.getID()) {
        case MouseEvent.MOUSE_CLICKED:
          mouseListener.mouseClicked(evt);
          break;
        case MouseEvent.MOUSE_ENTERED:
          mouseListener.mouseEntered(evt);
          break;
        case MouseEvent.MOUSE_EXITED:
          mouseListener.mouseExited(evt);
          break;
        case MouseEvent.MOUSE_PRESSED:
          mouseListener.mousePressed(evt);
          break;
        case MouseEvent.MOUSE_RELEASED:
          mouseListener.mouseReleased(evt);
          break;
      }
    super.processMouseEvent(evt);
  }

  /**
   * Process mouse motion events for this component. It will call the
   * methods (mouseDragged() etc) in the added mouse motion listeners.
   * @param evt the dispatched mouse event
   */
  public void processMouseMotionEvent(MouseEvent evt) {
    // handle mouse motion events
    if (mouseMotionListener != null)
      switch (evt.getID()) {
        case MouseEvent.MOUSE_DRAGGED:
          mouseMotionListener.mouseDragged(evt);
          break;
        case MouseEvent.MOUSE_MOVED:
          mouseMotionListener.mouseMoved(evt);
          break;
      }
    super.processMouseMotionEvent(evt);
  }

  private ActionListener actionListener;

  /**
   * Add a key listener to the VDU. This is necessary to be able to receive
   * keyboard input from this component. It is a prerequisite for a
   * lightweigh component.
   * @param listener the key listener
   */
  public void addActionListener(ActionListener listener) {
    actionListener = AWTEventMulticaster.add(actionListener, listener);
    enableEvents(AWTEvent.ACTION_EVENT_MASK);
  }

  /**
   * Remove key listener from the VDU. It is a prerequisite for a
   * lightweigh component.
   * @param listener the key listener to remove
   */
  public void removeActionListener(ActionListener listener) {
    actionListener = AWTEventMulticaster.remove(actionListener, listener);
  }

  public void processActionEvent(ActionEvent evt) {
      //System.out.println("process Action Event");
      // handle mouse motion events
      switch (evt.getID()) {
        case ActionEvent.ACTION_PERFORMED:
	  actionPerformed(evt);
          break;
      }
      //super.processActionEvent(evt);
  }


  private KeyListener keyListener;

  /**
   * Add a key listener to the VDU. This is necessary to be able to receive
   * keyboard input from this component. It is a prerequisite for a
   * lightweigh component.
   * @param listener the key listener
   */
  public void addKeyListener(KeyListener listener) {
    keyListener = AWTEventMulticaster.add(keyListener, listener);
    enableEvents(AWTEvent.KEY_EVENT_MASK);
  }

  /**
   * Remove key listener from the VDU. It is a prerequisite for a
   * lightweigh component.
   * @param listener the key listener to remove
   */
  public void removeKeyListener(KeyListener listener) {
    keyListener = AWTEventMulticaster.remove(keyListener, listener);
  }

  /**
   * Process key events for this component.
   * @param evt the dispatched key event
   */
  public void processKeyEvent(KeyEvent evt) {
    if (keyListener != null)
      switch (evt.getID()) {
        case KeyEvent.KEY_PRESSED:
          keyListener.keyPressed(evt);
          break;
        case KeyEvent.KEY_RELEASED:
          keyListener.keyReleased(evt);
          break;
        case KeyEvent.KEY_TYPED:
          keyListener.keyTyped(evt);
          break;
      }
    // consume TAB keys if they originate from our component
    if (evt.getKeyCode() == KeyEvent.VK_TAB && evt.getSource() == this)
      evt.consume();
    super.processKeyEvent(evt);
  }

  FocusListener focusListener;

  public void addFocusListener(FocusListener listener) {
    focusListener = AWTEventMulticaster.add(focusListener, listener);
  }

  public void removeFocusListener(FocusListener listener) {
    focusListener = AWTEventMulticaster.remove(focusListener, listener);
  }

  public void processFocusEvent(FocusEvent evt) {
    if (focusListener != null)
      switch (evt.getID()) {
        case FocusEvent.FOCUS_GAINED:
	  setBorder(2, true);
          focusListener.focusGained(evt);
	  redraw();
          break;
        case FocusEvent.FOCUS_LOST:
	  setBorder(2, false);
          focusListener.focusLost(evt);
	  redraw();
          break;
      }
    super.processFocusEvent(evt);
  }

  private int getModifiers(KeyEvent e) {
    return
            (e.isControlDown() ? VDUInput.KEY_CONTROL : 0) |
            (e.isShiftDown() ? VDUInput.KEY_SHIFT : 0) |
            (e.isAltDown() ? VDUInput.KEY_ALT : 0) |
            (e.isActionKey() ? VDUInput.KEY_ACTION : 0);

  }
}


