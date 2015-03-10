/*
 * --LICENSE NOTICE--
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of license of Jsch
 * (http://www.jcraft.com/jsch/).
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
 * --LICENSE NOTICE--
 */

package de.mud.jta.plugin;

import de.mud.jta.Plugin;
import de.mud.jta.FilterPlugin;
import de.mud.jta.PluginBus;
import de.mud.jta.PluginConfig;
import de.mud.jta.VisualPlugin;

import de.mud.jta.event.ConfigurationListener;
import de.mud.jta.event.OnlineStatusListener;
import de.mud.jta.event.TerminalTypeRequest;
import de.mud.jta.event.WindowSizeRequest;
import de.mud.jta.event.LocalEchoRequest;

import de.mud.ssh.SshIO;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

//import java.io.*;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.File;

import de.mud.jta.event.SocketListener;
import de.mud.jta.event.OnlineStatus;

import com.jcraft.jsch.JSch;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.ProxyHTTP;
import com.jcraft.jsch.ProxySOCKS5;
import com.jcraft.jsch.Channel;
import com.jcraft.jsch.Session;
import com.jcraft.jsch.UserInfo;
import com.jcraft.jsch.UIKeyboardInteractive;

import de.mud.jta.Applet;

/**
 * Secure Shell plugin for the Java Telnet Application (JTA). This is a plugin
 * to be used instead of Telnet for secure remote terminal sessions over
 * insecure networks.
 * <p>
 * This plugin works in conjuction with JSch, a open source client site pure
 * java implementation of the SSH protocol version 2 with a BSD styled 
 * license.
 * </p><p>
 * This plugin was written mainly because JSch is a more complete
 * implementation of SSH2 than the SSH plugin that comes with JTA. JSch comes 
 * with proxy and socks5 support, zlib compression and X11 forwarding. After 
 * copying the compiled class file of this source to de.mud.jta.plugin, it is
 * invoked like this:
 * </p><p><code>
 * java -cp <include jta and jsch here> de.mud.jta.Main -plugins Status,Jsch,Terminal [hostname]
 * </code></p><p>
 * Known Bugs:<br/><ul>
 * <li>Port always defaults to 22 and cannot be changed.</li>
 * <li>No SFTP support.</li>
 * </ul></p><p>
 * <b>Maintainer:</b> Thomas Pasch <thomas.pasch@gmx.de>
 * </p>
 *
 * @see <a href="http://www.sourceforge.net/projects/jta/">Java Telnet Application</a>
 * @see <a href="http://www.jcraft.com/jsch/">Jsch homepage</a>
 * @see <a href="http://www.openssh.org/">free Open Source version of SSH</a>
 * @see <a href="http://www.ssh.com/">Official SSH homepage</a>
 * @version $Id: Jsch.java,v 1.18 2005/03/03 17:05:34 regina Exp $
 * @author Thomas Pasch <thomas.pasch@gmx.de>
 */
public class Jsch extends Plugin implements FilterPlugin, VisualPlugin {
    
    private static final String MENU = "JSch";
    private static final String MENU_HTTP_PROXY = "Http Proxy ...";
    private static final String MENU_SOCKS_PROXY = "Socks Proxy ...";
    private static final String MENU_X11 = "X11 ...";
    private static final String MENU_LOCAL_PORT = "Local Port ...";
    private static final String MENU_REMOTE_PORT = "Remote Port ...";
    
    private static final JSch jSch_ = new JSch();
    
    private String proxyHttpHost_ = null;
    private int proxyHttpPort_;
    private String proxySOCKS5Host_ = null;
    private int proxySOCKS5Port_;
    private String xHost_ = null;
    private int xPort_;
    
    protected FilterPlugin source;
    // protected SshIO handler;
    protected Session session_;
    private Channel channel_;
    private InputStream in_;
    private OutputStream out_;
    private String host_;
    private int port_;
    public boolean bio_enabled;
    public boolean auth_none;
    public boolean req_auth;	//[RK]Jul/27/04 - OLD WEB
    
    // protected String user, pass;
    
    private final static int debug = 0;
    
    private volatile boolean auth = false;
    
    protected MyUserInfo userInfo_;
    
    /**
     * Create a new ssh plugin.
     */
    public Jsch(final PluginBus bus, final String id) {
        super(bus, id);
	
        // create a new telnet protocol handler
        //    handler = new SshIO() {
        //      /** get the current terminal type */
        //      public String getTerminalType() {
        //        return (String)bus.broadcast(new TerminalTypeRequest());
        //      }
        //      /** get the current window size */
        //      public Dimension getWindowSize() {
        //        return (Dimension)bus.broadcast(new WindowSizeRequest());
        //      }
        //      /** notify about local echo */
        //      public void setLocalEcho(boolean echo) {
        //        bus.broadcast(new LocalEchoRequest(echo));
        //      }
        //      /** write data to our back end */
        //      public void write(byte[] b) throws IOException {
        //        source.write(b);
        //      }
        //    };
        
        bus.registerPluginListener(new ConfigurationListener() {
		public void setConfiguration(PluginConfig config) {
                    userInfo_ = new MyUserInfo(null, null, null); 
		}
	    });
        
        // reset the protocol handler just in case :-)
        bus.registerPluginListener(new OnlineStatusListener() {
			public void online() {
		//[RK]Aug/09/04 - bug#2199 - get passwd in the second time
		newauthPopup:
		while (true)  {
		    	//modification for apm, append consoleid
         		if ((userInfo_ != null) && (Applet.noAuthPopup == true)) {
            		String user;
            		if (Applet.consoleChosen != null) {
               			user = Applet.user + ":" + Applet.consoleChosen;
            		} else {
               			if (Applet.portChosenSsh != null && Applet.portChosenSsh.equals("22") == false) {
                  			user = Applet.user + ":" + Applet.portChosenSsh;
               			} else {
                  			user = Applet.user;
               			}
            		}
            		userInfo_.setUser(user);
            		userInfo_.setPassword(Applet.password);
					req_auth = false;
				} else {
            		String user;
                  	user = Applet.user;
            		userInfo_.setUser(user);
					req_auth = true;
				}

		    if (Applet.AuthType != null) {
               	if (Applet.AuthType.compareTo("bio") == 0) {
				    bio_enabled=true;
			    	auth_none=false;
				}	 
				// Removed for PRD 21904. No reauthentication after user logs into web
				// reinclude to old WEB
				else if (Applet.AuthType.compareTo("none") == 0) {
			    			bio_enabled=false;
			    			auth_none=true;
					  } else {
			    			bio_enabled=false;
			    			auth_none=false;
					  }

		    }
		    // Modify the version string if biometric is enabled
		    if (bio_enabled) {
		    	com.jcraft.jsch.Session.version="JSCH-0.1.14-biometric";
				System.out.println("changing version to " +com.jcraft.jsch.Session.version);
		    } else {
				com.jcraft.jsch.Session.version="JSCH-0.1.14";
		    }
		    
		    String user = null, pass = null, passp=null;
		    if (userInfo_ != null) {
				user = userInfo_.getUser();
         		if (!req_auth) {
					pass = userInfo_.getPassword();
					passp= userInfo_.getPassphrase();
				}
				//System.out.println("USER: " + user);
				//System.out.println("pass: " + pass);
				//System.out.println("passp: " + passp);
		    }

		    if (bio_enabled || req_auth) {
				final Frame frame = new Frame("SSH User Authentication");
				Panel panel = new Panel(new GridLayout(3,1));
				panel.add(new Label("SSH Authorization required"));
				frame.add("North", panel);
				if (bio_enabled) {
					 //System.out.println("CreateGrid");
					if (req_auth) {
			    		panel = new Panel(new GridLayout(3,2));
					} else {
			   	 		panel = new Panel(new GridLayout(1,2));	
					}
				} else if (auth_none) {
			    			panel = new Panel(new GridLayout(1,2));
						} else {
			    			panel = new Panel(new GridLayout(2,2));
						}
				final TextField login = new TextField(user, 10);
				final TextField passw = new TextField(pass, 10);
				final TextField passph = new TextField(10);
				login.addActionListener(new ActionListener() {
                        public void actionPerformed(ActionEvent evt) {
                            passw.requestFocus();
                        }
			    });
				passw.setEchoChar('*');
				passph.setEchoChar('*');
				if (req_auth) {
					panel.add(new Label("User name")); panel.add(login);
					if (!auth_none) {
						panel.add(new Label("Password")); panel.add(passw);
					}
				}
				if (bio_enabled) {
			    	panel.add(new Label("Passphrase")); panel.add(passph);
				}
				frame.add("Center", panel);
				panel = new Panel();
				Button cancel = new Button("Cancel");
				Button ok = new Button("Login");
				ActionListener enter = new ActionListener() {
					public void actionPerformed(ActionEvent evt) {
					//[RK]Mar/03/05 - portChosenSsh has the local/remote TCP port.
					if (bio_enabled || (Applet.portChosenSsh == null) 
						|| (Applet.portChosenSsh.equals("22") == true)) {
               				userInfo_ = new MyUserInfo(login.getText(), passw.getText(),passph.getText());
					} else {
               				userInfo_ = new MyUserInfo(login.getText() + ":" + Applet.portChosenSsh, passw.getText(),null);
					}

                // modification for APM, if consoleChosen is set, this is
                // used by apms modified ssh server
                // if (Applet.consoleChosen != null) 

                  	if (req_auth) {
                    	String loginStr;
               			if (Applet.portChosenSsh.equals("22") == false) {
                  			loginStr = login.getText() + ":" + Applet.portChosenSsh;
               			} else {
                  			loginStr = login.getText();
            			}
                     	userInfo_.setUser(loginStr);
                  	}

				//System.out.println("USER 2: " + userInfo_.getUser());
				//System.out.println("pass 2: " + userInfo_.getPassword());
				//System.out.println("passp 2: " + userInfo_.getPassphrase());
				    frame.dispose();
				    connect();
					}
			    };
				ok.addActionListener(enter);
				passw.addActionListener(enter);
				cancel.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent evt) {
				    	frame.dispose();
					bus.broadcast(new OnlineStatus(false));
					}
			    });
				panel.add(cancel);
				panel.add(ok);
				frame.add("South", panel);
			
				frame.pack();
				frame.show();
				frame.setLocation(frame.getToolkit().getScreenSize().width/2 -
					  frame.getSize().width/2,
					  frame.getToolkit().getScreenSize().height/2 -
					  frame.getSize().height/2);
				if(user != null) {
			    	passw.requestFocus();
				}
		    } else {
			//error(user+":"+pass);
			
			//  handler.setLogin(user);
			//  handler.setPassword(pass);
			//userInfo_ = new MyUserInfo(user, pass, passph);
			//String user = userInfo_.getUser();
                	//user = user + ":" + Applet.consoleChosen;
			//userInfo_.setUser(user);
				connect();
			//[RK]Aug/09/04 - bug#2199 - get passwd in the second time
			if ((auth == false) && (Applet.noAuthPopup == true)){ 
				Applet.noAuthPopup = false;
				continue newauthPopup;
			}
		    }
			break;
		} // while (true)
		}
		
		private void connect() {
		    if (auth) {
			throw new IllegalStateException();
		    }
		    if (bio_enabled) {
			try {
			    String cmd="/Program Files/MXI/ClipDrive Bio/unlock";
			    Process proc=Runtime.getRuntime().exec(cmd);
			} catch (Exception e) {
			    e.printStackTrace();
			}
			keysearch k=new keysearch();
	
			while (! k.find_keys()) {
			    try {
				Thread.sleep(1);
			    } catch (Exception e) {
				System.out.println("sleep failed");
			    }
			}
			System.out.println("Found key filename: "+k.getkeyfile());
                   	String dsaidfile=k.getkeyfile();
                        String knownhostfile=k.getdriveletter()+":/known_hosts";
                        File file1=new File(knownhostfile);
                        File file2=new File(dsaidfile);

			try {
			    jSch_.setKnownHosts(knownhostfile);
		    	    System.out.println("Using dsa id file: "+dsaidfile);
		    	    System.out.println("Using known hosts file: "+knownhostfile);	
		    	    jSch_.addIdentity(dsaidfile);
			} catch (Exception e) {
			    e.printStackTrace();
			}
		    }
		    try {
			//[RK]Mar/03/05 - 
			session_ = jSch_.getSession(userInfo_.getUser(), host_, port_);
			session_.setUserInfo(userInfo_);
			if (isProxyHttp()) {
			    session_.setProxy(new ProxyHTTP(proxyHttpHost_, proxyHttpPort_));
			}
			else if (isProxySOCKS5()) {
			    session_.setProxy(new ProxySOCKS5(proxySOCKS5Host_, proxySOCKS5Port_));
			}
			if (isXForwarding()) {
			    session_.setX11Host(xHost_);
			    session_.setX11Port(xPort_);
                    	}
                    	session_.connect();
		    } catch (JSchException e) {
			//[RK]Aug/09/04 - bug#2199 - get passwd in the second time
         		if ((userInfo_ != null) && (Applet.noAuthPopup == true)) {
		    		auth = false;
				return;
			}

			System.out.println("Login failed.  Please restart the application and try again");
			final Frame frame = new Frame("Login Failed");
			Panel panel = new Panel(new GridLayout(2,1));
			panel.add(new Label("Login failed.  Please click OK and restart the applet to try again."));
			frame.add("North", panel);
			panel = new Panel();
			Button cancel = new Button("OK");
			cancel.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent evt) {
				    frame.dispose();
				}
			    });
			panel.add(cancel);
			frame.add("South", panel);
			
			frame.pack();
			frame.show();
			frame.setLocation(frame.getToolkit().getScreenSize().width/2 -
					  frame.getSize().width/2,
					  frame.getToolkit().getScreenSize().height/2 -
					  frame.getSize().height/2);
			bus.broadcast(new OnlineStatus(false));
			throw new RuntimeException(e);
		    }
		    try {
			channel_ = session_.openChannel("shell");
		    } catch (JSchException e) {
			throw new RuntimeException(e);
		    }
		    try {
			in_ = channel_.getInputStream();
			out_ = channel_.getOutputStream();
		    }
		    catch (IOException e) {
			e.printStackTrace();
			throw new RuntimeException(e);
		    }
		    try {
			channel_.connect();
		    } catch (JSchException e) {
			throw new RuntimeException(e);
		    }
		    auth = true;
		}
		
		public void offline() {
		    try {
			if (in_ != null) in_.close();
			if (out_ != null) out_.close();
		    }
		    catch (IOException e) {
			e.printStackTrace();
			throw new RuntimeException(e);
		    }
		    finally {
                    auth=false;
                    userInfo_ = null;
                    //if (channel_ != null) channel_.disconnect();
                    if (session_ != null) session_.disconnect();
		    }
		}
	    });
        bus.registerPluginListener(new SocketListener() {
		public void connect(String host, int port) {
		    host_ = host;
		    port_ = port;
		    //System.out.println("connect to host " + host_ + " port " + port_);
		    bus.broadcast(new OnlineStatus(true));
		}
		public void disconnect() {
		    bus.broadcast(new OnlineStatus(false));
		}
        });
    }
    
    public void setFilterSource(FilterPlugin source) {
        if(debug>0) System.err.println("ssh: connected to: "+source);
        this.source = source;
    }
    
    public FilterPlugin getFilterSource() {
        return source;
    }
    
    private byte buffer[];
    private int pos=0;
    
    /**
     * Read data from the backend and decrypt it. This is a buffering read
     * as the encrypted information is usually smaller than its decrypted
     * pendant. So it will not read from the backend as long as there is
     * data in the buffer.
     * @param b the buffer where to read the decrypted data in
     * @return the amount of bytes actually read.
     */
    public int read(byte[] b) throws IOException {
        // we don't want to read from the pipeline without authorization
        while(!auth) try {
            Thread.sleep(1000);
        } catch(InterruptedException e) {
            e.printStackTrace();
        }
        if (in_ == null) {
          bus.broadcast(new OnlineStatus(false)); 
          return -1;
        }
 
	int n = in_.read(b);
	if (n < 0) 
          bus.broadcast(new OnlineStatus(false)); 
	//System.out.println("Read "+n+"bytes");
        return n;
    }
    
    /**
     * Write data to the back end. This hands the data over to the ssh
     * protocol handler who encrypts the information and writes it to
     * the actual back end pipe.
     * @param b the unencrypted data to be encrypted and sent
     */
    public void write(byte[] b) throws IOException {
        // no write until authorization is done
        if(!auth) return;
        for (int i=0;i<b.length;i++) {
            switch (b[i]) {
                case 10: /* \n -> \r */
                    b[i] = 13;
                    break;
            }
        }
	//System.out.println("Wrote bytes");
        out_.write(b);
    }

    public JComponent getPluginVisual() {
        // JTextArea comp = new JTextArea("getPluginVisual");
        return null;
    }
    
    public JMenu getPluginMenu() {
        final JMenu menu = new JMenu(MENU);
        //
        JMenuItem item = new JMenuItem(MENU_HTTP_PROXY);
        item.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                String foo = getProxyHttpHost();
                int bar = getProxyHttpPort();
                String proxy = JOptionPane.showInputDialog(
                    "HTTP proxy server (hostname:port)",
                    ((foo!=null&&bar!=0)? foo + ":" + bar : ""));
                if (proxy == null) return;
                if (proxy.length() == 0 ) {
                    setProxyHttp(null, 0);
                    return;
                }
                try{
                    foo=proxy.substring(0, proxy.indexOf(':'));
                    bar=Integer.parseInt(proxy.substring(proxy.indexOf(':')+1));
                    if(foo!=null){
                        setProxyHttp(foo, bar);
                    }
                }
                catch(Exception ee){
                }
            }
        });
        menu.add(item);
        //
        item = new JMenuItem(MENU_SOCKS_PROXY);
        item.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                String foo=getProxySOCKS5Host();
                int bar=getProxySOCKS5Port();
                String proxy=
                JOptionPane.showInputDialog(
                    "SOCKS5 server (hostname:1080)",
                    ((foo!=null&&bar!=0)? foo+":"+bar : ""));
                if(proxy==null)return;
                if(proxy.length()==0){
                    setProxySOCKS5(null, 0);
                    return;
                }              
                try{
                    foo=proxy.substring(0, proxy.indexOf(':'));
                    bar=Integer.parseInt(proxy.substring(proxy.indexOf(':')+1));
                    if(foo!=null){
                        setProxySOCKS5(foo, bar);
                    }
                }
                catch(Exception ee){
                }
            }
        });
        menu.add(item);
        //
        item = new JMenuItem(MENU_X11);
        item.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                String display=JOptionPane.showInputDialog(
                    "XDisplay name (hostname:0)",
                    (getXHost() == null) ? "": (getXHost() + ":" + getXPort()));
                try{
                    if(display!=null){
                        String host = display.substring(0, display.indexOf(':'));
                        int port = Integer.parseInt(display.substring(display.indexOf(':')+1));
                        setXForwarding(host, port);
                    }
                }
                catch(Exception ee){
                    setXForwarding(null, -1);
                }
            }
        });
        menu.add(item);
        //
        item = new JMenuItem(MENU_LOCAL_PORT);
        item.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                if (session_ == null){
                    JOptionPane.showMessageDialog(menu.getParent(), 
                        "Establish the connection before this setting.");
                    return;
                }                
                try{
                    String title = "Local port forwarding (port:host:hostport)";
                    String foo = JOptionPane.showInputDialog(title, "");
                    if (foo == null) return;
                    int port1 = Integer.parseInt(foo.substring(0, foo.indexOf(':')));
                    foo = foo.substring(foo.indexOf(':') + 1);
                    String host = foo.substring(0, foo.indexOf(':'));
                    int port2 = Integer.parseInt(foo.substring(foo.indexOf(':') + 1));
                    setPortForwardingL(port1, host, port2);
                }
                catch(Exception ee){
                }
            }
        });
        menu.add(item);
        //
        item = new JMenuItem(MENU_REMOTE_PORT);
        item.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                if (session_ == null) {
                    JOptionPane.showMessageDialog(menu.getParent(),
                        "Establish the connection before this setting.");
                    return;
                }                
                try{
                    String title="Remote port forwarding (port:host:hostport)";
                    String foo=JOptionPane.showInputDialog(title, "");
                    if (foo == null) return;
                    int port1 = Integer.parseInt(foo.substring(0, foo.indexOf(':')));
                    foo = foo.substring(foo.indexOf(':') + 1);
                    String host = foo.substring(0, foo.indexOf(':'));
                    int port2 = Integer.parseInt(foo.substring(foo.indexOf(':') + 1));
                    setPortForwardingR(port1, host, port2);
                }
                catch(Exception ee){
                }
            }
        });
        menu.add(item);
        //
        return menu;
    }
    
    private boolean isProxyHttp() {
        return proxyHttpHost_ != null;
    }
    
    private String getProxyHttpHost() {
        return proxyHttpHost_;
    }
    
    private int getProxyHttpPort() {
        return proxyHttpPort_;
    }
    
    private void setProxyHttp(String host, int port) {
        proxyHttpHost_ = host;
        switch (port) {
            case -1:
                proxyHttpHost_ = null;
                proxyHttpPort_ = port;
                break;
            case 0:
                proxyHttpPort_ = 3128;
                break;
            default:
                proxyHttpPort_ = port;
        }
    }
    
    private boolean isProxySOCKS5() {
        return proxySOCKS5Host_ != null;
    }
    
    private String getProxySOCKS5Host() {
        return proxySOCKS5Host_;
    }
    
    private int getProxySOCKS5Port() {
        return proxySOCKS5Port_;
    }
    
    private void setProxySOCKS5(String host, int port) {
        proxySOCKS5Host_ = host;
        switch (port) {
            case -1:
                proxySOCKS5Host_ = null;
                proxySOCKS5Port_ = port;
                break;
            case 0:
                proxySOCKS5Port_ = 3128;
                break;
            default:
                proxySOCKS5Port_ = port;
        }
    }
    
    private boolean isXForwarding() {
        return xHost_ != null;
    }
    
    private String getXHost() {
        return xHost_;
    }
    
    private int getXPort() {
        return xPort_;
    }
    
    private void setXForwarding(String host, int port) {
        xHost_ = host;
        switch (port) {
            case -1:
                xHost_ = null;
                xPort_ = port;
                break;
            default:
                xPort_ = port;
        }
    }
    
    private void setPortForwardingR(int port1, String host, int port2) {
        try{ 
            session_.setPortForwardingR(port1, host, port2); 
        }
        catch (JSchException e) {
            e.printStackTrace();
            throw new RuntimeException(e);
        }
    }
    
    private void setPortForwardingL(int port1, String host, int port2) {
	try {
	    session_.setPortForwardingL(port1, host, port2);
	} catch (JSchException e) {
	    throw new RuntimeException(e);
	}
    }
       
}

final class MyUserInfo implements UserInfo, UIKeyboardInteractive {
    
    private String user_;
    private String password_;
    private String passphrase_;


    public String[] promptKeyboardInteractive(String destination, String name, String instruction, String[] prompt, boolean[] echo){
           String[] res = new String[1];
	   res[0] = this.password_;
	   return res;
    }

    
    public MyUserInfo(String user, String password, String passphrase) {
        user_ = user;
        password_ = password;
        passphrase_ = passphrase;
	// Modified for testing Erik
    }
    
    public String getUser() {
        return user_;
    }
    
    public void setUser(String user_) {
        this.user_ = user_;
    }

    public String getPassphrase() {
        return passphrase_;
    }
    
    public String getPassword() {
        return password_;
    }
    
    public void setPassword(String password_) {
        this.password_ = password_;
    }

    public boolean promptPassphrase(String message) {
        return true;
    }
    
    public boolean promptPassword(String message) {
        return true;
    }
    
    public boolean promptYesNo(String message) {
        return true;
    }
    
    public void showMessage(String message) {
    }
}

class keysearch {
    String keyfile=null;
    char driveletter='f';

    String getkeyfile() {
	return(keyfile);
    }

    String getdriveletter() {
	return(String.valueOf(driveletter));
    }
    
    boolean find_keys() {
	String keyfiles[]=new String[22];
	//char drivelet='e';
	char drivelet='d';	//[RK]Aug/9/04 
	for (int i=0; i < 22; i++) {
	    keyfiles[i]=String.valueOf(drivelet);
	    String keyfilename=keyfiles[i]+":/id_dsa";
//	    System.out.println("Trying file: "+keyfilename);
	    File kfile=new File(keyfilename);
	    if (kfile.canRead()) {
		System.out.println("Setting key file name to: "+keyfilename);
		keyfile=keyfilename;
		driveletter=drivelet;
		return(true);
	    }
	    drivelet++;
	}

	return(false);
    }
}
