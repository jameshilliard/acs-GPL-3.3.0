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
package de.mud.jta;

import de.mud.jta.event.AppletRequest;
import de.mud.jta.event.FocusStatusListener;
import de.mud.jta.event.OnlineStatusListener;
import de.mud.jta.event.ReturnFocusRequest;
import de.mud.jta.event.SocketRequest;
import de.mud.jta.event.SoundListener;

import javax.swing.JApplet;
import javax.swing.JFrame;
import javax.swing.RootPaneContainer;
import java.awt.BorderLayout;
import java.awt.Button;
import java.awt.Component;
import java.awt.Container;
import java.awt.Menu;
import java.awt.Frame;
import java.awt.Window;
import java.awt.MenuBar;
import java.awt.MenuItem;
import java.awt.MenuShortcut;
import java.awt.Toolkit;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import java.awt.print.PrinterJob;

import javax.swing.*;
import java.awt.event.*;

import java.awt.PrintJob;
import java.awt.datatransfer.Clipboard;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.net.URL;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.Map;
import java.util.Properties;
import java.util.Vector;

/**
 * <B>The Java Telnet Applet</B><P>
 * This is the implementation of whole set of applications. It's modular
 * structure allows to configure the software to act either as a sophisticated
 * terminal emulation and/or, adding the network backend, as telnet
 * implementation. Additional modules provide features like scripting or an
 * improved graphical user interface.<P>
 * This software is written entirely in Java<SUP>tm</SUP>.<P>
 * This is the <I>Applet</I> implementation for the software. It initializes
 * the system and adds all needed components, such as the telnet backend and
 * the terminal front end.
 * <P>
 * <B>Maintainer:</B> Matthias L. Jugel
 *
 * @version $Id: Applet.java,v 1.9 2004/06/24 00:10:22 jennifer Exp $
 * @author Matthias L. Jugel, Marcus Mei�ner
 */
public class Applet extends JApplet {

  private final static int debug = 0;
  public static Applet app;

  private String frameTitle = null;
  private RootPaneContainer appletFrame;

  /** holds the defaults */
  private Properties options = new Properties();

  /** hold the common part of the jta */
  private Common pluginLoader;

  /** hold the host and port for our connection */
  //private String host, port;
  private String host;

  // variables below are for Bio
  public static String portChosenSsh = null, breakSeq = null;
  public static String NatClustering = null;
  public static String AuthType = null;
  //public static String UserName = null;

  // variables for preferrably APM and ACS
  public static String UnitType = null;
  public static String protocol = null;
  public static String port = null;

  // variables below are for APM + connecting to box
  public static String consoleChosen = null;
  public static String user = null;
  public static String password = null;
  public static String currHost = null;
  public static boolean noAuthPopup = false;

/** disconnect on leave, this is to force applets to break the connection */
  private boolean disconnect = true;
  /** connect on startup, this is to force applets to connect on detach */
  private boolean connect = false;
  /** close the window (if it exists) after the connection is lost */
  private boolean disconnectCloseWindow = true;

  private Plugin focussedPlugin;
  private Clipboard clipboard;
  private boolean online = false;

  /**
   * Read all parameters from the applet configuration and
   * do initializations for the plugins and the applet.
   */
  public void init() {
                System.out.println("new version v5");
		this.app = this;
		if (debug > 0) System.err.println("Applet: init() ");
		if (pluginLoader == null) {
			String value;

			// try to load the local configuration and merge it with the defaults
			if ((value = getParameter("config")) != null) {
				Properties appletParams = new Properties();
				URL url = null;
				try {
					url = new URL(value);
				} catch (Exception e) {
					try {
						url = new URL(getCodeBase() + value);
					} catch (Exception ce) {
						System.err.println("jta: could not find config file: " + ce);
					}
				}

				if (url != null) {
					try {
						if (debug > 0) System.out.println("loading applet parms");
						//appletParams.load(Applet.class.getResourceAsStream("/de/mud/jta/" + value));
						appletParams.load(Applet.class.getResourceAsStream(value));
						Enumeration ape = appletParams.keys();
						while (ape.hasMoreElements()) {
							String key = (String) ape.nextElement();
							if (debug > 0) System.out.println("put key= " + key + " val= " + appletParams.getProperty(key));
							options.put(key, appletParams.getProperty(key));
						}
					} catch (Exception e) {
						try {
							appletParams.load(url.openStream());
							Enumeration ape = appletParams.keys();
							while (ape.hasMoreElements()) {
								String key = (String) ape.nextElement();
								if (debug > 0) System.out.println("Key: " + key + ", Value: " + appletParams.getProperty(key));
								options.put(key, appletParams.getProperty(key));
							}
						} catch (Exception e2) {
							System.err.println("jta: could not load config file: " + e2);
						}
					}
				}
			}

		// set the host to our code base, no other hosts are allowed anyway
		host = getCodeBase().getHost();

		// Change for Biometric [EL] Get authentication type
		try {
			AuthType=getParameter("authtype");
		} catch (Exception e) {
			System.out.println("authtype parameter not available");
		}
		user=getParameter("username");
      UnitType=getParameter("unit");
      protocol=getParameter("proto");

		// If serial port protocol = socket_ssh. P_SOCKET_SSH returns 'Y', which
		// equals '89'. 
		if (protocol.equals("89")) { 
			breakSeq = getParameter("breakseq");	 
			NatClustering = getParameter("NatClustering");
			if (NatClustering == null) {
				// SSH plugin will use portChosenSsh when authenticating
				// and old fashion clustering is being used.
				portChosenSsh = getParameter("port");	 
				port = "22";  // ssh port to connect to
			} else {
				//[RK]Mar/03/05 :  remote TCP port
				portChosenSsh = NatClustering;	 
				port = getParameter("port");
			}
		} else { 
			NatClustering = null;
			portChosenSsh = null;
			port = getParameter("port");
		}

      //change for apm
      if (UnitType != null && UnitType.equals("apm") 
			&& (getParameter("parma") != null) && (getParameter("parmb") != null)) {
         String parm1 = getParameter("parma");
         String parm2 = getParameter("parmb");
         String[] dec = Utils.decrypt(parm2, parm1);
         user = dec[0];
         password = dec[1];
         consoleChosen = dec[2];
      }

      if (UnitType != null && UnitType.equals("acs")) {
         noAuthPopup = true;
         String parm1 = getParameter("parma");
         String parm2 =getParameter("parmb");

         byte[] myByte1 = Utils.hexDecode(parm1);
         parm1 = new String(myByte1);
         parm1 = Utils.myDecrypt(null, parm1, true);
         //System.out.println("Decrypted parm1: " + parm1); 


         byte[] myByte2 = Utils.hexDecode(parm2);
         parm2 = new String(myByte2);
         //System.out.println("Decrypt hex parm2: [" + parm2 + "]");
         password = Utils.myDecrypt(parm1, parm2, false);
         //System.out.println("Decrypt Password: [" + password + "]");
      } else {
            //System.out.println("acs false. noauthpopup false");
      }

		// see if there are parameters in the html to override properties
		parameterOverride(options);

		// configure the application and load all plugins
		pluginLoader = new Common(options);

				
		if (port == null)
			port = "23";

		if ((new Boolean(options.getProperty("Applet.connect"))
					.booleanValue()))
			connect = true;
		if (!(new Boolean(options.getProperty("Applet.disconnect"))
					.booleanValue()))
			disconnect = false;

		if (!(new Boolean(options.getProperty("Applet.disconnect.closeWindow"))
					.booleanValue()))
			disconnectCloseWindow = false;

		frameTitle = options.getProperty("Applet.detach.title");

		if ((new Boolean(options.getProperty("Applet.detach"))).booleanValue()) {
			if (frameTitle == null) {
				appletFrame = new JFrame("jta: " + host + (port.equals("23")?"":" " + port));
			} else {
				appletFrame = new JFrame(frameTitle);
			}
		} else {
			appletFrame = this;
		}
		appletFrame.getContentPane().setLayout(new BorderLayout());

		Map componentList = pluginLoader.getComponents();
		Iterator names = componentList.keySet().iterator();
		while (names.hasNext()) {
			String name = (String) names.next();
			Component c = (Component) componentList.get(name);
			if ((value = options.getProperty("layout." + name)) != null) {
				appletFrame.getContentPane().add(value, c);
			} else {
				System.err.println("jta: no layout property set for '" + name + "'");
				System.err.println("jta: ignoring '" + name + "'");
			}
		}

		pluginLoader.registerPluginListener(new SoundListener() {
			public void playSound(URL audioClip) {
				Applet.this.getAudioClip(audioClip).play();
			}
		});

		pluginLoader.broadcast(new AppletRequest(this));

                
                // register a focus status listener, so we know when a plugin got focus
                pluginLoader.registerPluginListener(new FocusStatusListener() {
                        public void pluginGainedFocus(Plugin plugin) {
                                if (Applet.debug > 0)
                                        System.err.println("Applet: " + plugin + " got focus");
                                focussedPlugin = plugin;
                        }

                        public void pluginLostFocus(Plugin plugin) {
                                // we ignore the lost focus
                                if (Applet.debug > 0)
                                        System.err.println("Applet: " + plugin + " lost focus");
                        }
                });
                
		if (appletFrame != this) {
			final String startText = options.getProperty("Applet.detach.startText");
			final String stopText = options.getProperty("Applet.detach.stopText");
			final Button close = new Button();

			// this works for Netscape only!
			Vector privileges =
					Common.split(options.getProperty("Applet.Netscape.privilege"), ',');
			Class privilegeManager = null;
			Method enable = null;
			try {
				privilegeManager =
						Class.forName("netscape.security.PrivilegeManager");
				enable = privilegeManager
						.getMethod("enablePrivilege", new Class[]{String.class});
			} catch (Exception e) {
				System.err.println("Applet: This is not Netscape ...");
			}

			if (privilegeManager != null && enable != null && privileges != null)
				for (int i = 0; i < privileges.size(); i++)
				try {
					enable.invoke(privilegeManager,
								new Object[]{privileges.elementAt(i)});
					System.out.println("Applet: access for '" +
									 privileges.elementAt(i) + "' allowed");

				} catch (Exception e) {
					System.err.println("Applet: access for '" +
									 privileges.elementAt(i) + "' denied");
				}

			// set up the clipboard
			getClipboard();

			if ((new Boolean(options.getProperty("Applet.detach.immediately"))
						.booleanValue())) {
				if ((new Boolean(options.getProperty("Applet.detach.fullscreen"))
							.booleanValue()))
					((JFrame) appletFrame)
							.setSize(appletFrame.getContentPane().getToolkit().getScreenSize());
				else
					((JFrame) appletFrame).pack();

				((JFrame) appletFrame).show();
				pluginLoader.broadcast(new SocketRequest(host, Integer.parseInt(port)));
				pluginLoader.broadcast(new ReturnFocusRequest());
				close.setLabel(startText != null ? stopText : "Disconnect");
			} else
				close.setLabel(startText != null ? startText : "Connect");

			close.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent evt) {
					if (((JFrame) appletFrame).isVisible()) {
						pluginLoader.broadcast(new SocketRequest());
						((JFrame) appletFrame).setVisible(false);
						close.setLabel(startText != null ? startText : "Connect");
					} else {
						if (frameTitle == null)
							((JFrame) appletFrame)
											.setTitle("jta: " + host + (port.equals("23")?"":" " + port));
						if ((new Boolean(options.getProperty("Applet.detach.fullscreen"))
									.booleanValue()))
							((JFrame) appletFrame)
											.setSize(appletFrame.getContentPane().getToolkit().getScreenSize());
						else
							((JFrame) appletFrame).pack();
						((JFrame) appletFrame).show();
						getAppletContext().showStatus("Trying " + host + " " + port + " ...");
						pluginLoader.broadcast(new SocketRequest(host, Integer.parseInt(port)));
						pluginLoader.broadcast(new ReturnFocusRequest());
						close.setLabel(stopText != null ? stopText : "Disconnect");
					}
				}
			});

			getContentPane().setLayout(new BorderLayout());
			getContentPane().add("Center", close);

			// add a menu bar
			System.out.println("menuBar.1 = " + options.getProperty("Applet.detach.menuBar"));
			MenuBar mb = new MenuBar();
			Menu file = new Menu("File");
			file.setShortcut(new MenuShortcut(KeyEvent.VK_F, true));
			MenuItem tmp;
			file.add(tmp = new MenuItem("Connect"));
			tmp.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent evt) {
				pluginLoader.broadcast(new SocketRequest(host,
														 Integer.parseInt(port)));
				}
			});
			file.add(tmp = new MenuItem("Disconnect"));
			tmp.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent evt) {
				pluginLoader.broadcast(new SocketRequest());
				}
			});
			//file.add(new MenuItem("-"));
			//REMIND. disabled currently
			//file.add(tmp = new MenuItem("Print"));
			tmp.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent evt) {
					if (pluginLoader.getComponents().get("Terminal") != null) {
						PrintJob printJob =
								appletFrame.getContentPane().getToolkit()
								.getPrintJob((JFrame) appletFrame, "JTA Terminal", null);
						((Component) pluginLoader.getComponents().get("Terminal"))
								.print(printJob.getGraphics());
						printJob.end();
					}
				}
			});
			//file.add(new MenuItem("-"));
			//file.add(tmp = new MenuItem("Exit"));
			tmp.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent evt) {
					((JFrame) appletFrame).setVisible(false);
					pluginLoader.broadcast(new SocketRequest());
					close.setLabel(startText != null ? startText : "Connect");
				}
			});
			mb.add(file);

			Menu edit = new Menu("Edit");
			edit.setShortcut(new MenuShortcut(KeyEvent.VK_E, true));
			edit.add(tmp = new MenuItem("Copy"));
			tmp.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent evt) {
					if (debug > 2)
						System.err.println("Applet: copy: " + focussedPlugin);
					if (focussedPlugin instanceof VisualTransferPlugin)
						((VisualTransferPlugin) focussedPlugin).copy(clipboard);
				}
			});
			edit.add(tmp = new MenuItem("Paste"));
			tmp.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent evt) {
					if (debug > 2)
						System.err.println("Applet: paste: " + focussedPlugin);
					if (focussedPlugin instanceof VisualTransferPlugin)
						((VisualTransferPlugin) focussedPlugin).paste(clipboard);
				}
			});
			mb.add(edit);

			Map menuList = pluginLoader.getMenus();
			names = menuList.keySet().iterator();
			/** while (names.hasNext()) {
				String name = (String) names.next();
				mb.add((Menu) menuList.get(name));
			} */

			Menu help = new Menu("Help");
			help.setShortcut(new MenuShortcut(KeyEvent.VK_HELP, true));
			help.add(tmp = new MenuItem("General"));
			tmp.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					Help.show(appletFrame.getContentPane(), options.getProperty("Help.url"));
				}
			});
			mb.setHelpMenu(help);

			// only add the menubar if the property is true
			System.out.println("menuBar = " + options.getProperty("Applet.detach.menuBar"));
			if ((new Boolean(options.getProperty("Applet.detach.menuBar"))
						.booleanValue()))
				((JFrame) appletFrame).setMenuBar(mb);

			// add window closing event handler
			try {
				((JFrame) appletFrame).addWindowListener(new WindowAdapter() {
					public void windowClosing(WindowEvent evt) {
						pluginLoader.broadcast(new SocketRequest());
						((JFrame) appletFrame).setVisible(false);
						close.setLabel(startText != null ? startText : "Connect");
					}
				});
			} catch (Exception e) {
				System.err.println("Applet: could not set up Window event listener");
				System.err.println("Applet: you will not be able to close it");
			}

			pluginLoader.registerPluginListener(new OnlineStatusListener() {
				public void online() {
					if (debug > 0) System.err.println("Terminal: online");
					online = true;
					if (((JFrame) appletFrame).isVisible() == false)
						((JFrame) appletFrame).setVisible(true);
				}

				public void offline() {
					if (debug > 0) System.err.println("Terminal: offline");
					online = false;
					if (disconnectCloseWindow) {
						((JFrame) appletFrame).setVisible(false);
						close.setLabel(startText != null ? startText : "Connect");
					}
				}
			});

                        //REMIND: moved out of this loop as is independent of
                        //if appletFrame == this or not
			// register a focus status listener, so we know when a plugin got focus
			/** pluginLoader.registerPluginListener(new FocusStatusListener() {
				public void pluginGainedFocus(Plugin plugin) {
					if (Applet.debug > 0)
						System.err.println("Applet: " + plugin + " got focus");
					focussedPlugin = plugin;
				}

				public void pluginLostFocus(Plugin plugin) {
					// we ignore the lost focus
					if (Applet.debug > 0)
						System.err.println("Applet: " + plugin + " lost focus");
				}
			});
                         */

		} else {
                        setupCutNPaste();
			// if we have no external frame use this online status listener
			pluginLoader.registerPluginListener(new OnlineStatusListener() {
				public void online() {
					if (debug > 0) System.err.println("Terminal: online");
					online = true;
				}

				public void offline() {
					if (debug > 0) System.err.println("Terminal: offline");
					online = false;
				}
			});
                }
		}
  }
  
  public void getClipboard() {
    // set up the clipboard
    System.out.println("cut and paste enabled...");
    try {
            clipboard = appletFrame.getContentPane().getToolkit().getSystemClipboard();
            System.out.println("clipboard setup clipboard = " + clipboard);
            System.err.println("Applet: acquired system clipboard: " + clipboard);
    } catch (Exception e) {
            System.err.println("Applet: system clipboard access denied: " +
                                                                                            ((e instanceof InvocationTargetException) ?
                                                                                            ((InvocationTargetException) e).getTargetException() : e));
            // e.printStackTrace();
    } finally {
            if (clipboard == null) {
            System.out.println("clipboard null");
            System.err.println("Applet: copy & paste only within the JTA");
            clipboard = new Clipboard("de.mud.jta.Main");
            }
    }
  }

  public void setupCutNPaste() {
        getClipboard();
	
	//REMIND. user mb later, right now no top menu system
	//need to uncomment setJMenuBar(mb) call below to activeate this menu,
	//currently no menu shown
        JMenuBar mb = new JMenuBar();
        JMenu file = new JMenu("File");
        JMenuItem tmp;

        file.add(tmp = new JMenuItem("Connect"));
        tmp.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent evt) {
                pluginLoader.broadcast(new SocketRequest(host,Integer.parseInt(port))); }
        });
        file.add(tmp = new JMenuItem("Disconnect"));
        tmp.addActionListener(new ActionListener() { 
		public void actionPerformed(ActionEvent evt) {
                	pluginLoader.broadcast(new SocketRequest());
                }
        });
        file.add(new JMenuItem("-"));

	tmp = new JMenuItem("Print");
        file.add(tmp);
        tmp.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent evt) {
                        if (pluginLoader.getComponents().get("Terminal") != null) {
                                //need Frame
				Component c = (Component) pluginLoader.getComponents().get("Terminal");
                                Frame f = getFrame(c);
                                //PrintJob printJob = appletFrame.getContentPane().getToolkit().getPrintJob(f, "JTA Terminal", null);
                                PrintJob printJob = Toolkit.getDefaultToolkit().getPrintJob(f, "JTA Terminal", null);
                                appletFrame.getContentPane().print(printJob.getGraphics());
                                //appletFrame.getContentPane().printAll(printJob.getGraphics());
                                printJob.end();
                        }
                }
        });

	tmp = new JMenuItem("Exit");
	tmp.setMnemonic(KeyEvent.VK_X);
        file.add(tmp);
	final JApplet a = this;
        tmp.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent evt) {
                        pluginLoader.broadcast(new SocketRequest());
		 	appletFrame.getContentPane().setVisible(false);
			//stop();
			//getFrame(appletFrame.getRootPane()).setVisible(false);
			//getFrame(appletFrame.getRootPane()).dispose();
			//setVisible(false);
			//getWindow(a).setVisible(false);
			//getFrame(a.getContentPane()).setVisible(false);
			//getFrame(a.getContentPane()).dispose();
			//getWindow(a.getContentPane()).dispose();
			//System.exit(0);
			//getWindow(a).dispose();
			//System.exit(0);
			getFrame(a).dispose();
                }
        });
        
        mb.add(file);

        JMenu edit = new JMenu("Edit");
	JMenuItem cpy = new JMenuItem("Copy");
	cpy.setMnemonic(KeyEvent.VK_C);
        edit.add(cpy);
        cpy.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent evt) {
                        if (debug > 2) 
                                System.err.println("Applet: copy: " + focussedPlugin);
                        if (focussedPlugin instanceof VisualTransferPlugin) {
                            ((VisualTransferPlugin) focussedPlugin).copy(clipboard);
                        } else {
                            System.out.println("no focus");
                        }
                }
        });

	JMenuItem pst = new JMenuItem("Paste");
	pst.setMnemonic(KeyEvent.VK_P);
        edit.add(pst);
        pst.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent evt) {
                        if (debug > 2)
                                System.err.println("Applet: paste: " + focussedPlugin);
                        if (focussedPlugin instanceof VisualTransferPlugin)
                                ((VisualTransferPlugin) focussedPlugin).paste(clipboard);
                }
        });
        mb.add(edit);

        //Map menuList = pluginLoader.getMenus();
        //names = menuList.keySet().iterator();
        /** while (names.hasNext()) {
                String name = (String) names.next();
                mb.add((Menu) menuList.get(name));
        } */

        //JMenu help = new JMenu("Help");
        //help.setShortcut(new JMenuShortcut(KeyEvent.VK_HELP, true));
        //help.add(tmp = new JMenuItem("General"));
        //tmp.addActionListener(new ActionListener() {
        //        public void actionPerformed(ActionEvent e) {
                        //Help.show(appletFrame.getContentPane(), options.getProperty("Help.url"));
                //}
        //});
        //mb.add(help);

        // only add the menubar if the property is true
        //System.out.println("menuBar = " + options.getProperty("Applet.detach.menuBar"));
        //if ((new Boolean(options.getProperty("Applet.detach.menuBar"))
        //                        .booleanValue()))
        //        ((JFrame) appletFrame).setMenuBar(mb);

	//REMIND. uncomment this line to make the menu active
        //appletFrame.getRootPane().setJMenuBar(mb);
  }

  /**
   * Start the applet. Connect to the remote host.
   */
  public void start() {
	if (!online && (appletFrame == this || connect)) {
	  if (debug > 0) System.err.println("start(" + host + ", " + port + ")");
			getAppletContext().showStatus("Trying " + host + " " + port + " ...");
			pluginLoader.broadcast(new SocketRequest(host, Integer.parseInt(port)));
			pluginLoader.broadcast(new ReturnFocusRequest());
		}
  }

  /**
   * Stop the applet and disconnect.
   */
  public void stop() {
		if (online && disconnect) {
			if (debug > 0) System.err.println("stop()");
			pluginLoader.broadcast(new SocketRequest());
		}
  }
  
   public Frame getFrame(Component c) {
	return getFrame(c.getParent());
   }
  
   // methods added for apms copy and paste support
   // and popup menus for apm
   public Frame getFrame(Container comp) {
      		System.out.println("getFrame comp = " + comp); 
		Container c = comp;
		try {
			while( !(c instanceof Frame) ) {
				c = c.getParent();
				System.out.println("getting frame c = " + c);
			}
			return (Frame)c;
		} catch ( NullPointerException e ) {
			return null;
		}
    }

   /** public javax.swing.JFrame getJFrame(javax.swing.JComponent comp) {
	return getJFrame(comp.getParent());
   }

   public javax.swing.JFrame getJFrame(javax.swing.JContainer comp) {
      		System.out.println("getFrame comp = " + comp); 
		Container c = comp;
		try {
			while( !(c instanceof JFrame) ) {
				c = c.getParent();
				System.out.println("getting frame c = " + c);
			}
			return (JFrame)c;
		} catch ( NullPointerException e ) {
			return null;
		}
    }
   */
    
    public Window getWindow(Container comp) {
       
      		System.out.println("getWindow comp = " + comp); 
		Container c = comp;
		try {
			while( !(c instanceof Window) )  {
				c = c.getParent();
				System.out.println("getting window c = " + c);
			}
			return (Window)c;
		} catch ( NullPointerException e ) {
			return null;
		}
    }


  /**
   * Override any properties that are found in the configuration files
   * with possible values found as applet parameters.
   * @param options the loaded configuration file properties
   */
  private void parameterOverride(Properties options) {
		Enumeration e = options.keys();
		while (e.hasMoreElements()) {
			String key = (String) e.nextElement(), value = getParameter(key);
			if (value != null) {
				System.out.println("Applet: overriding value of " + key + " with " + value);
				// options.setProperty(key, value);
				options.put(key, value);
			}
		}
  }
}
