/*
 * This file is part of "The Java Telnet Application".
 *
 * (c) Matthias L. Jugel, Marcus Meiﬂner 1996-2002. All Rights Reserved.
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

package de.mud.jta.plugin;

import de.mud.jta.FilterPlugin;
import de.mud.jta.Plugin;
import de.mud.jta.PluginBus;
import de.mud.jta.PluginConfig;
import de.mud.jta.VisualPlugin;
import de.mud.jta.Applet;
import de.mud.jta.event.ConfigurationListener;
import de.mud.jta.event.SocketRequest;
import de.mud.jta.event.TelnetCommandRequest;
import de.mud.jta.event.ReturnFocusRequest;
import de.mud.jta.plugin.Terminal;


import java.awt.Component;
import java.awt.Container;
import javax.swing.JApplet;

import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JMenu;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import java.awt.Cursor;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.StreamTokenizer;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;

/**
 * Implementation of a programmable button bar to be used as a plugin
 * in the Java(tm) Telnet Applet/Application. The button bar is configured
 * using a input file that contains the setup for the button bar.<P>
 * A typical setup file may look like:
 * <PRE>
 * #
 * # Example for writing a button bar config file.
 * #
 * # The syntaxt for defining buttons, input fields and breaks is as follows:
 * #
 * # - defining a button:
 * # A button is defined by giving the keyword 'button' followed by the text
 * # of the button and the command that should be sent when pressing the
 * # button. If the command contains whitespace characters, enclode it in
 * # quote (") characters!
 * #
 * button		Connect		"\$connect(\@host@,\@port@)"
 * #
 * # - defining a label:
 * # A labvel is defined by giving the keyword 'label' followed by the text
 * # of the label. If the label contains whitespace characters, enclode it in
 * # quote (") characters!
 * #
 * label 		"Hello User"
 * #
 * # - defining an input field:
 * # An input field is defined just like the button above, but it has one more
 * # parameter, the size of the input field. So you define it, by giving the
 * # keyword 'input' followed by the name of the input field (for reference)
 * # followed by the size of the input field and optionally a third parameter
 * # which is the initial text to be displayed in that field.
 * #
 * input		host	20	"tanis"
 * stretch
 * input		port	4	"23"
 * #
 * # Now after the button and two input fields we define another button which
 * # will be shown last in the row. Order is significant for the order in
 * # which the buttons and fields appear.
 * #
 * button		Disconnect	"\\$disconnect()" break
 * #
 * # To implement an input line that is cleared and sends text use this:
 * # The following line send the text in the input field "send" and appends
 * # a newline.
 * input		send	20	"\\@send@\n"	"ls"
 * #
 * # - Defining a choice
 * # A choice is defined just like the button above, but it has multiple
 * # text/command pairs. If the text or command contain whitespace characters,
 * # enclose them in quote (") characters. The text and command data may be
 * # spread over several lines for better readability. Make the first command
 * # empty because it is initially selected, and choosing it will have no
 * # effect until some other item has been chosen.
 * #choice       "- choose -"   ""
 * #             "Text 1"       "Command 1"
 * #             "Text 2"       "Command 2"
 * #             "Text 3"       "Command 3"
 * # etc...

 * </PRE>
 * Other possible keywords are <TT>break</TT> which does introduce a new
 * line so that buttons and input fields defined next will appear in a new
 * line below and <TT>stretch</TT> to make the just defined button or input
 * field stretched as far as possible on the line. That last keyword is
 * useful to fill the space.
 *
 * @version $Id: ButtonBar.java,v 1.9 2004/07/28 12:58:33 regina Exp $
 * @author  Matthias L. Jugel, Marcus Meiﬂner
 */
public class ButtonBar extends Plugin
        implements FilterPlugin, VisualPlugin, ActionListener, ListSelectionListener {

  /** the panel that contains the buttons and input fields */
  protected JPanel panel = new JPanel();

  // these tables contain our buttons and fields.
  private Map buttons = null;
  private Map choices = null;
  private Map fields = null;
	private char ESC = 27;
	private boolean win = false;

  // the switch for clearing input fields after enter
  private boolean clearFields = true;

  /**
   * Initialize the button bar and register plugin listeners
   */
  public ButtonBar(PluginBus bus, final String id) {
    super(bus, id);

    // configure the button bar
    bus.registerPluginListener(new ConfigurationListener() {
      public void setConfiguration(PluginConfig cfg) {
        String file = cfg.getProperty("ButtonBar", id, "setup");
				if (file.equals("/winbuttonbar.conf"))
					win = true;
        clearFields =
                (new Boolean(cfg.getProperty("ButtonBar", id, "clearFields")))
                .booleanValue();

        // check for the setup file
        if (file == null) {
          ButtonBar.this.error("no setup file");
          return;
        }

        StreamTokenizer setup = null;
        InputStream is = null;

        try {
          is = getClass().getResourceAsStream(file);
        } catch (Exception e) {
          // ignore any errors here
        }

        // if the resource access fails, try URL
        if (is == null)
          try {
            is = new URL(file).openStream();
          } catch (Exception ue) {
            ButtonBar.this.error("could not find: " + file);
            return;
          }

        // create a new stream tokenizer to read the file
        try {
          InputStreamReader ir = new InputStreamReader(is);
          setup = new StreamTokenizer(new BufferedReader(ir));
        } catch (Exception e) {
          ButtonBar.this.error("cannot load " + file + ": " + e);
          return;
        }

        setup.commentChar('#');
        setup.quoteChar('"');

        fields = new HashMap();
        buttons = new HashMap();
        choices = new HashMap();

        GridBagLayout l = new GridBagLayout();
        GridBagConstraints c = new GridBagConstraints();
        panel.setLayout(l);
        c.fill = GridBagConstraints.BOTH;

        int token;
        int ChoiceCount = 0;
        JList list;
        // parse the setup file
        try {
          while ((token = setup.nextToken()) != StreamTokenizer.TT_EOF) {
            switch (token) {
              case StreamTokenizer.TT_WORD:
                // reset the constraints
                c.gridwidth = 1;
                c.weightx = 0.0;
                c.weighty = 0.0;
                // keyword found, parse arguments
                if (setup.sval.equals("button")) {
                  if ((token = setup.nextToken()) != StreamTokenizer.TT_EOF) {
                    String descr = setup.sval;	//button label
                    if ((token = setup.nextToken()) != StreamTokenizer.TT_EOF) {
                      JButton b = new JButton(descr);	//creates a button with text, descr
                      buttons.put(b, setup.sval);	 //associate button function value (sval) with key (b)
                      b.addActionListener(ButtonBar.this);
                      l.setConstraints(b, constraints(c, setup));
                      panel.add(b);
                    } else
                      ButtonBar.this.error(descr + ": missing button command");
                  } else
                    ButtonBar.this.error("unexpected end of file");
                } else if (setup.sval.equals("label")) {
                  if ((token = setup.nextToken()) != StreamTokenizer.TT_EOF) {
                    String descr = setup.sval;
                    JLabel b = new JLabel(descr);
                    l.setConstraints(b, constraints(c, setup));	//set the constraints for component button. 
                    panel.add(b);
                  } else
                    ButtonBar.this.error("unexpected end of file");
                  /* choice - new stuff added APS 07-dec-2001 for Choice
                   * buttons
                   * Choice info is held in the choices hash. There are two
                   * sorts of hash entry:
                   * 1) for each choices button on the terminal, key=choice
                   *    object, value=ID ("C1.", "C2.", etc)
                   * 2) for each item, key=ID+user's text (eg "C1.opt1"),
                   *    value=user's command
                   */

                } else if (setup.sval.equals("choice")) {
                  ChoiceCount++;
                  String ident = "C" + ChoiceCount + ".";
                  list = new JList();
                  choices.put(list, ident);
                  list.addListSelectionListener(ButtonBar.this);// Choices use ItemListener, not Action
                  l.setConstraints(list, constraints(c, setup));
                  panel.add(list);
                  while ((token = setup.nextToken()) != StreamTokenizer.TT_EOF) {
                    if (isKeyword(setup.sval)) {// Got command ... Back off. Returns true if sval is a javakeyword and not just a variable
                      setup.pushBack();
                      break;
                    }
                    String descr = setup.sval;  // This is the hash key. choice name	
                    token = setup.nextToken();
                    if (token == StreamTokenizer.TT_EOF) {
                      ButtonBar.this.error("unexpected end of file");
                    } else {
                      String value = setup.sval;		// choice command
                      if (isKeyword(value)) {   // Missing command - complain but continue
                        System.err.println(descr + ": missing choice command");
                        setup.pushBack();
                        break;
                      }
                      System.out.println("choice: name='" + descr + "', value='" + value);
                      list.add(descr, new JLabel(descr));
                      choices.put(ident + descr, value);
                    }
                  }
                  ButtonBar.this.error("choices hash: " + choices);
                } else if (setup.sval.equals("input")) {
                  if ((token = setup.nextToken()) != StreamTokenizer.TT_EOF) {
                    String descr = setup.sval;	//variable
                    if ((token = setup.nextToken()) ==
                            StreamTokenizer.TT_NUMBER) {
                      int size = (int) setup.nval;	//size of input
                      String init = "", command = "";
                      token = setup.nextToken();
                      if (isKeyword(setup.sval))
                        setup.pushBack();
                      else
                        command = setup.sval;	//command function
                      token = setup.nextToken();
                      if (isKeyword(setup.sval)) {
                        setup.pushBack();
                        init = command;
                      } else
                        init = setup.sval;	//text command
                      JTextField t = new JTextField(init, size);
                      if (!init.equals(command)) {
                        buttons.put(t, command);
                        t.addActionListener(ButtonBar.this);
                      }
                      fields.put(descr, t); //parameter variable (key), text command (value)
                      l.setConstraints(t, constraints(c, setup));
                      panel.add(t);
                    } else
                      ButtonBar.this.error(descr + ": missing field size");
                  } else
                    ButtonBar.this.error("unexpected end of file");
                }
                break;
              default:
                ButtonBar.this.error("syntax error at line " + setup.lineno());
            }
          }
        } catch (IOException e) {
          ButtonBar.this.error("unexpected error while reading setup: " + e);
        }
        panel.validate();
      }
    });
  }

  private GridBagConstraints constraints(GridBagConstraints c,
                                         StreamTokenizer setup)
          throws IOException {
    if (setup.nextToken() == StreamTokenizer.TT_WORD)
      if (setup.sval.equals("break"))
        c.gridwidth = GridBagConstraints.REMAINDER;
      else if (setup.sval.equals("stretch"))
        c.weightx = 1.0;
      else
        setup.pushBack();
    else
      setup.pushBack();
    return c;
  }

  public void valueChanged(ListSelectionEvent evt) {
    String tmp, ident;
    if ((ident = (String) choices.get(evt.getSource())) != null) {
      // It's a choice - get the text from the selected item
      JList list = (JList) evt.getSource();
      tmp = (String) choices.get(ident + list.getSelectedValue());
      if (tmp != null) processEvent(tmp);
    }
  }

  public void actionPerformed(ActionEvent evt) {
    String tmp;
    if ((tmp = (String) buttons.get(evt.getSource())) != null) 
		processEvent(tmp);
  }

  private boolean readySAC() {
    byte [] chn0 = { (byte)ESC, (byte)'\t', (byte)'0', (byte)'\r', (byte)'\n', (byte)'\r', (byte)'\n'};
    byte [] ctrlC = { 0x03 };
    byte [] command;
    int tries = 50; /* 50 x 0.1 second == 5 seconds */
    boolean isReady = false;
    Component cursorTarget;

    if (Terminal.sac)
	return true;

    if (Terminal.more)
	command = ctrlC;
    else
	command = chn0;

    try {
	write(command);
    } catch (IOException e) {
	error("readySAC() failed");
	return false;
    }

    cursorTarget = ((JApplet)panel.getTopLevelAncestor()).getRootPane();
    cursorTarget.setCursor(new Cursor(Cursor.WAIT_CURSOR));
    while (--tries > 0) {
        try {
                Thread.sleep(100);
        } catch (InterruptedException e) {
            error("sleep: " + e);
	    isReady = false;
	    break;
        }
	if (Terminal.sac)
	{
		isReady = true;
		break;
	}
    }

    cursorTarget.setCursor(null);
    if (!isReady)
	error("Couldn't switch back to SAC.");
    return isReady;
  }

  private void writeWithDelay(String str) {
    try {
        write(str.getBytes());
        try {
            Thread.sleep(40);
        } catch (InterruptedException e) {
            error("sleep: " + e);
        }
    } catch (IOException e) {
        error("writeWithDelay() failed");
    }
  }

  private void processEvent(String tmp) {
    String cmd = "", function = null;
    int idx = 0, oldidx = 0;
    boolean sac = Terminal.sac;
    boolean more = Terminal.more;

    bus.broadcast(new ReturnFocusRequest());

    while ((idx = tmp.indexOf('\\', oldidx)) >= 0 &&
            ++idx <= tmp.length()) {
      cmd += tmp.substring(oldidx, idx - 1);
      switch (tmp.charAt(idx)) {
        case 'b':
          cmd += "\b";
          break;
        case 'e':
          cmd += "";
          break;
        case 'n':
          cmd += "\n";
          break;
        case 'r':
          cmd += "\r";
          break;
        case '$':
          {
            int ni = tmp.indexOf('(', idx + 1);
            if (ni < idx) {
              error("ERROR: Function: missing '('");
              break;
            }
            if (ni == ++idx) {
              error("ERROR: Function: missing name");
              break;
            }
            function = tmp.substring(idx, ni);	//get function name
            idx = ni + 1;
            ni = tmp.indexOf(')', idx);
            if (ni < idx) {
              error("ERROR: Function: missing ')'");
              break;
            }
            tmp = tmp.substring(idx, ni); //tmp now has the parameters of the function
            idx = oldidx = 0;
            continue;
          }
        case '@':
          {
            int ni = tmp.indexOf('@', idx + 1);
            if (ni < idx) {
              error("ERROR: Input Field: '@'-End Marker not found");
              break;
            }
            if (ni == ++idx) {
              error("ERROR: Input Field: no name specified");
              break;
            }
            String name = tmp.substring(idx, ni); //parameter 1 found
            idx = ni;
            JTextField t;
            if (fields == null || (t = (JTextField) fields.get(name)) == null) { //look in field hash table for parameter
              error("ERROR: Input Field: requested input \"" +
                    name + "\" does not exist");
              break;
            }
            cmd += t.getText();	//cmd now has the command to execute
            if (clearFields) t.setText("");
            break;
          }
        default :
          cmd += tmp.substring(idx, ++idx);
      }
      oldidx = ++idx;
    }

    if (oldidx <= tmp.length()) cmd += tmp.substring(oldidx, tmp.length());

    if (function != null) {
      if (function.equals("break")) {
		System.out.println("break sent to telnet command");
		if (Applet.breakSeq == null)
			bus.broadcast(new TelnetCommandRequest((byte) 243)); // BREAK
		else {
			try {
				//System.out.println("Send break seq: " + Applet.breakSeq);
				write((Applet.breakSeq).getBytes());
			} catch (IOException e) {
            	error("break(): failed");
            }
		}
        return;
      }
      if (function.equals("exit")) {
        try {
          System.exit(0);
        } catch (Exception e) {
          error("cannot exit: " + e);
        }
      }
      if (function.equals("connect")) {
        String address = null;
        int port = -1;
        try {
          if ((idx = cmd.indexOf(",")) >= 0) {
            try {
              port = Integer.parseInt(cmd.substring(idx + 1, cmd.length()));
            } catch (Exception e) {
              port = -1;
            }
            cmd = cmd.substring(0, idx);
          }
          if (cmd.length() > 0) address = cmd;
          if (address != null)
            if (port != -1) 
              bus.broadcast(new SocketRequest(address, port));
            else
              bus.broadcast(new SocketRequest(address, 23));
          else
            error("connect: no address");
        } catch (Exception e) {
          error("connect(): failed");
          e.printStackTrace();
        }
      } else if (function.equals("switchchn")) {
				if (cmd.length() > 0) {
					if (!cmd.equals("0") && readySAC()) {
						writeWithDelay("ch -si " + cmd + "\r\n");
					}
				} else {
					error("ERROR: No channel number specified");
				}
      } else if (function.equals("closechn")) {
				if (cmd.length() > 0) { 
					if (readySAC())
						writeWithDelay("ch -ci " + cmd + "\r\n");
				} else {
					error("ERROR: No channel number specified");
				}
			} else if (function.equals("kill")) {
				if (cmd.length() > 0) { 
					if (readySAC())
						writeWithDelay("k " + cmd + "\r\n");
				} else {
					error("ERROR: No process ID specified");
				}
      } else if (function.equals("lower")) {
				if (cmd.length() > 0) { 
					if (readySAC())
						writeWithDelay("l " + cmd + "\r\n");
				} else {
					error("ERROR: No process ID specified");
				}
      } else if (function.equals("raise")) {
				if (cmd.length() > 0) { 
					if (readySAC())
						writeWithDelay("r " + cmd + "\r\n");
				} else {
					error("ERROR: No process ID specified");
				}
      } else if (function.equals("limit")) {
				String pid = null, mb = null;
				try {
					if (cmd.length() > 0) {
						if ((idx = cmd.indexOf(",")) >= 0) {
							pid = cmd.substring(0, idx);
							if (pid == null || pid.length() == 0) {
								error("Limit Memory: no PID");
								pid = null;
							}
							cmd = cmd.substring(idx + 1, cmd.length());
						}
						mb = cmd;
						if (mb == null || mb.length() == 0) {
							error("Limit Memory: no PID");
							mb = null;
						}
					} else {
						error("Limit Memory: No field specified");
					}
				} catch (Exception e) {
					error("Limit Memory(): failed");
					e.printStackTrace();
				}
				if (readySAC())
					writeWithDelay("m " + pid + " " + mb + "\r\n");
      } else if (function.equals("setTimeDate")) {
				String month = null, day = null, year = null;
				String hour = null, minute = null;
				try {
					if (cmd.length() > 0) {
						if ((idx = cmd.indexOf(",")) >= 0) {
							month = cmd.substring(0, idx);
							if (month == null)
								error("setTimeDate: no month");
							if (month.length() != 2) {
								error("ERROR: Month is specified with two digits: \"" + month + "\"" );
								month = "mm";
							}
							cmd = cmd.substring(idx + 1, cmd.length());
						}
						if ((idx = cmd.indexOf(",")) >= 0) {
							day = cmd.substring(0, idx);
							if (day == null)
								error("setTimeDate: no day");
							if (day.length() != 2) {
								error("ERROR: Day is specified with two digits: \"" + day + "\"" );
								day = "dd";
							}
							cmd = cmd.substring(idx + 1, cmd.length());
						}
						if ((idx = cmd.indexOf(",")) >= 0) {
							year = cmd.substring(0, idx);
							if (year == null)
								error("setTimeDate: no year");
							if (year.length() != 4) {
								error("ERROR: Year is specified with four digits: \"" + year + "\"" );
								year = "yyyy";
							}
							cmd = cmd.substring(idx + 1, cmd.length());
						}	
						if ((idx = cmd.indexOf(",")) >= 0) {
							hour = cmd.substring(0, idx);
							if (hour == null)
								error("setTimeDate: no hour");
							if (hour.length() != 2) {
								error("ERROR: Hour is specified with two digits: \"" + hour + "\"" );
								hour = "hh";
							}
							cmd = cmd.substring(idx + 1, cmd.length());
							//System.out.println("CMD: " + cmd);
						}
						minute = cmd;
						if (minute == null)
							error("setTimeDate: no minute");
						if (minute.length() != 2) {
							error("ERROR: Minute is specified with two digits: \"" + minute + "\"" ); 
							minute = "mm";
						}
					} else {
						error("setTimeDate: No field specified");
					}
				} catch (Exception e) {
					error("setTimeDate(): failed");
					e.printStackTrace();
				}
				if (readySAC()) {
					writeWithDelay("s ");
//					writeWithDelay(month + "/" + day + "/" + year + " ");
					writeWithDelay(month + "/");
					writeWithDelay(day + "/");
					writeWithDelay(year + " ");
					writeWithDelay(hour + ":" + minute);
					writeWithDelay("\r\n");
				}
      } else if (function.equals("setIP")) {
				String netwkNum = null, ip = null, subnet = null, gtwy = null;
				try {
					if (cmd.length() > 0) {
						if ((idx = cmd.indexOf(",")) >= 0) {
							netwkNum = cmd.substring(0, idx);
							if (netwkNum == null)
								error("setIP: no network interface number");
							cmd = cmd.substring(idx + 1, cmd.length());
						}
						if ((idx = cmd.indexOf(",")) >= 0) {
							ip = cmd.substring(0, idx);
							if (ip == null)
								error("setIP: no ip");
							cmd = cmd.substring(idx + 1, cmd.length());
						}
						if ((idx = cmd.indexOf(",")) >= 0) {
							subnet = cmd.substring(0, idx);
							if (subnet == null)
								error("setIP: no subnet");
							cmd = cmd.substring(idx + 1, cmd.length());
						}	
						gtwy = cmd;
						if (gtwy == null)
							error("setIP: no gateway");
					} else {
						error("setIP: No field specified");
					}
				} catch (Exception e) {
					error("setIP(): failed");
					e.printStackTrace();
				}
				if (readySAC()) {
					writeWithDelay("i ");
					writeWithDelay(netwkNum + " ");
					writeWithDelay(ip + " ");
					writeWithDelay(subnet + " ");
					writeWithDelay(gtwy + "\r\n");
				}
      } else if (function.equals("disconnect")) {
	//System.out.println("disconnecting ...");
        bus.broadcast(new SocketRequest());
	//System.out.println("applet = " + getJApplet(panel));

	if (Applet.UnitType != null) { // OLD WEB
		java.applet.AppletContext ctx = getJApplet(panel).getAppletContext();
		System.out.println("ctx = " + ctx);
		try {
			if (Applet.UnitType != null && Applet.UnitType.equals("apm"))
				ctx.showDocument(new URL(Applet.app.getCodeBase(), "usermode/close.html"));
			else
				ctx.showDocument(new URL(Applet.app.getCodeBase(), "close.asp"));
		} catch (Exception e) {
			System.out.println("error showing document");
        	}
	}
      } else if (function.equals("detach")) {
        error("detach not implemented yet");
      } else
        error("ERROR: function not implemented: \"" + function + "\"");
      return;
    }
    if (cmd.length() > 0) {
			if (win) {
				if (readySAC())
					writeWithDelay(cmd);
			} else {
				try {
					write(cmd.getBytes());
				} catch(IOException e) {
					error("send: " + e);
				}
			}
		}
  }

  public JComponent getPluginVisual() {
    return panel;
  }

  public JMenu getPluginMenu() {
    return null;
  }


   public JApplet getJApplet(Component comp) {
        Container c = comp.getParent();
        return getJApplet(c);
   }

   // methods added for apms copy and paste support
   // and popup menus for apm
   public JApplet getJApplet(Container comp) {
        Container c = comp;
        try {
        while( !(c instanceof JApplet) ) c = c.getParent();
                return (JApplet)c;
        } catch ( NullPointerException e ) {
                return null;
        }
   }



  FilterPlugin source;

  public void setFilterSource(FilterPlugin source) {
    this.source = source;
  }

  public FilterPlugin getFilterSource() {
    return source;
  }

  public int read(byte[] b) throws IOException {
    return source.read(b);
  }

  public void write(byte[] b) throws IOException {
    source.write(b);
  }

  private static boolean isKeyword(String txt) {
    return (
            txt.equals("button") ||
            txt.equals("label") ||
            txt.equals("input") ||
            txt.equals("stretch") ||
            txt.equals("choice") ||
            txt.equals("break")
            );
  }
}
