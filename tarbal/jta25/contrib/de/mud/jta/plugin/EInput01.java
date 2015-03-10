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

package de.mud.jta.plugin;

import de.mud.jta.FilterPlugin;
import de.mud.jta.Plugin;
import de.mud.jta.PluginBus;
import de.mud.jta.VisualPlugin;

import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JMenu;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;

/**
 * An example plugin that creates a text area and sends the text entered
 * there to the remote host. The example explains how to create a filter
 * plugin (for sending only) and a visual plugin.
 * <P>
 * <B>Maintainer:</B> Matthias L. Jugel
 *
 * @version $Id: EInput01.java,v 1.5 2002/05/06 07:24:20 leo Exp $
 * @author Matthias L. Jugel, Marcus Mei�ner
 */

public class EInput01 extends Plugin
        implements FilterPlugin, VisualPlugin {

  protected JTextArea input;
  protected JButton send;
  protected JPanel panel;

  public EInput01(PluginBus bus, final String id) {
    super(bus, id);

    // create text field and send button
    input = new JTextArea(10, 30);
    send = new JButton("Send Text");

    // add action listener to send the text
    send.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent evt) {
        try {
          // calls the write from FilterPlugin interface
          write(input.getText().getBytes());
        } catch (Exception e) {
          System.err.println("EInput01: error sending text: " + e);
          e.printStackTrace();
        }
      }
    });

    // create a panel and put text field and button into it.
    panel = new JPanel(new BorderLayout());
    panel.add("Center", input);
    panel.add("South", send);
  }


  /** the source where we get data from */
  protected FilterPlugin source = null;

  public void setFilterSource(FilterPlugin plugin) {
    source = plugin;
  }

  public FilterPlugin getFilterSource() {
    return source;
  }

  /**
   * Read data from the filter plugin source and return the amount read.
   * We do not really do anything here
   * @param b the array where to read the bytes in
   * @return the amount of bytes actually read
   */
  public int read(byte[] b) throws IOException {
    return source.read(b);
  }

  /**
   * Write data to the filter plugin source. This method is used by the
   * visual components of the plugin to send data.
   */
  public void write(byte[] b) throws IOException {
    source.write(b);
  }

  /**
   * This method returns the visual part of the component to be displayed
   * by the applet or application at the specified location in the config
   * file.
   * @return a visual Component
   */
  public JComponent getPluginVisual() {
    return panel;
  }

  /**
   * If you want to have a menu configure it and return it here.
   * @return the plugin menu
   */
  public JMenu getPluginMenu() {
    return null;
  }
}
