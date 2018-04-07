/////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs - WPF gui written in C#                      //
//   - makes calls into C++\CLI shim dll                           //
//   - Shim is bound to a MockChannel static library               //
// Application: Remote Code Repository  - CSE 687 Project 4     	//
// Platform:    Asus R558U, Win 10 Student Edition, Visual studio 15//
// Author: Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017//
//		   Nitesh Bhutani, CSE687, nibhutani@syr.edu				//
//////////////////////////////////////////////////////////////////////

/*
Package Operations:
==================

This is backend code attach to the WPF GUI. 
It has different event handlers or functions 
which are executed when certain event take place at GUI.

*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;
using Microsoft.Win32; //FileDialog 
using WinForms = System.Windows.Forms; //FolderDialog 
using System.IO; //Folder, Directory 
using System.Diagnostics; //Debug.WriteLine 
using System.Xml.Linq;
using System.Xml;
namespace WPFgui
{
    /// <summary>
    /// Main Window Class
    /// </summary>
  public partial class MainWindow : Window
  {
    Shim shim ;//= new Shim();
    Thread tRcv = null;
    //Constructor
    public MainWindow()
    {
      InitializeClient();
      InitializeComponent();
      
    }

    //----------<Helper Function to process command line arguments and make the instance of shim> ---------------------------
    private void InitializeClient()
    {
            string[] args = Environment.GetCommandLineArgs();
            int clientPort = Convert.ToInt32(args[1].Substring(args[1].IndexOf(':') + 1));
            int serverPort = Convert.ToInt32(args[2].Substring(args[2].IndexOf(':') + 1));
            shim = new Shim(clientPort,serverPort);
        }
    
    //----------<Function which is executed when download button is clicked> ---------------------------
    private void downloadButton_Click(object sender, RoutedEventArgs e)
    {
        WinForms.FolderBrowserDialog folderDialog = new WinForms.FolderBrowserDialog();
        folderDialog.ShowNewFolderButton = false;
        folderDialog.SelectedPath = System.AppDomain.CurrentDomain.BaseDirectory;
        WinForms.DialogResult result = folderDialog.ShowDialog();
        if (result == WinForms.DialogResult.OK)
        {       String sPath = folderDialog.SelectedPath;
                DownloadFolderTxtBox.Text = sPath;
                string path = getParentPath(PublishFiles);
                if(path != string.Empty)
                {
                    string[] pathVector = path.Split('\\');
                    string fileName = pathVector[pathVector.Length - 2];
                    string path_ = string.Empty;
                    for (int i= 0; i < pathVector.Length-2; i++)
                    {
                        path_ += pathVector[i] + "\\";
                    } 
                    //send command
                    string msg = "Command:Download\nFile:"+fileName+"\nPath:" + path_ + "\n";
                    msg += "DownloadPath:" + sPath + "\n";
                    shim.PostMessage(msg);
                    Debug.WriteLine(msg);
                }
                else
                {
                    MessageBox.Show("Please select the file in \"Server publich Code repo\" treeview to download the file on client side.");
                }
        }
    }

    //----------<Function which is executed when upload button is clicked> ---------------------------
    private void uploadButton_Click(object sender, RoutedEventArgs e)
    {   string msg = string.Empty;
        TreeViewItem t = (TreeViewItem)CodeFiles.SelectedItem;
            if (t.Items.Count == 0 && t.Parent.GetType().ToString().Equals("System.Windows.Controls.TreeViewItem"))
            {   MessageBox.Show("Please select directory in Server Code Repo."); }
            else {    if (SelectFolder.IsChecked ?? false)
                {   WinForms.FolderBrowserDialog folderDialog = new WinForms.FolderBrowserDialog();
                    folderDialog.ShowNewFolderButton = false;
                    folderDialog.SelectedPath = System.AppDomain.CurrentDomain.BaseDirectory;
                    WinForms.DialogResult result = folderDialog.ShowDialog();
                    if (result == WinForms.DialogResult.OK)
                    {   String sPath = folderDialog.SelectedPath;
                        FileNameTextBox.Text = sPath;
                        string[] path_ = sPath.Split('\\');
                        string path = String.Join("\\", path_.Take(path_.Count() - 1).ToArray());
                        List<string> files = DirSearch(sPath);
                        foreach (string s in files)
                        {   string[] s_ = s.Split('\\');
                            string folderPath = String.Join("\\", s_.Take(s_.Count() - 1).ToArray());
                            string filename = s_[s_.Length - 1];
                            msg = "Command:Upload\nFile:" + filename + "\nPath:" + folderPath + "\\\n";
                            msg += "ServerPath:" + getParentPath(CodeFiles) + folderPath.Remove(0, path.Length + 1) + "\\\n";
                            shim.PostMessage(msg);
                        }
                    }
                }  else
                {   OpenFileDialog openFileDialog = new OpenFileDialog();
                    openFileDialog.Multiselect = true;
                    openFileDialog.Filter = "Header files (*.h)|*.h|Code Files (*.cpp)|*.cpp";
                    openFileDialog.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
                    if (openFileDialog.ShowDialog() == true)
                    {   foreach (string fileName in openFileDialog.FileNames)
                        {   FileNameTextBox.Text += fileName + ";";
                            string[] s_ = fileName.Split('\\');
                            string folderPath = String.Join("\\", s_.Take(s_.Count() - 1).ToArray());
                            string filename = s_[s_.Length - 1];
                            msg = "Command:Upload\nFile:" + filename + "\nPath:" + folderPath + "\\\n";
                            msg += "ServerPath:" + getParentPath(CodeFiles) + "\n";
                            Debug.WriteLine("#Debug: msg: " + msg);
                            shim.PostMessage(msg);
                        }
                    }
                }
                string msg_ = "Command:GetSourceCode\n";
                shim.PostMessage(msg_);
            }
    }

    //----------<Helper Function to search all the files in a particular directory> ---------------------------
    private List<String> DirSearch(string sDir)
    {
        List<String> files = new List<String>();
        try
        {
            foreach (string f in Directory.GetFiles(sDir))
            {
                files.Add(f);
            }
            foreach (string d in Directory.GetDirectories(sDir))
            {
                files.AddRange(DirSearch(d));
            }
        }
        catch (System.Exception excpt)
        {
            Debug.WriteLine(excpt.Message);
        }

        return files;
    }

    //----------<Function which is executed when Referesh Left button is clicked> ---------------------------
    private void RefButton1_Click(object sender, RoutedEventArgs e)
    {
            string msg = "Command:GetSourceCode\n";
            shim.PostMessage(msg);
            //string xml = "<dir name=\"serverSourceCodeRepo\"><file name=\"CLIShim.h\" /><file name=\"CodePublisher.h\" /><dir name=\"TA - TEST - Project - 2\"><dir name=\"TA - TEST - Project - 2\"><dir name=\"files\"><file name=\"Grandparent.h\" /><file name=\"Invalid.h\" /><file name=\"Parent.cpp\" /><file name=\"Parent.h\" /><file name=\"Test.cpp\" /><dir name=\"Child\"><file name=\"Child.cpp\" /><file name=\"Child.h\" /><file name=\"Child2.h\" /></dir></dir></dir></dir></dir>";
            
    }

    //----------<Helper Function to parse parent Nodes of selected Treeview Item> ---------------------------
    private  string getParentPath(TreeView tree_)
    {
            TreeViewItem t = (TreeViewItem)tree_.SelectedItem;
            List<string> folderPath = new List<string>();
            string Path = string.Empty;
            if (t != null)
            {
                while (t.Parent.GetType().ToString().Equals("System.Windows.Controls.TreeViewItem"))
                {
                    folderPath.Add(t.Header.ToString());
                    t = (TreeViewItem)t.Parent;
                }
                folderPath.Add(t.Header.ToString());
                folderPath.Reverse();
                foreach (string s in folderPath)
                {
                    Path += s + "\\";
                }
            }

            return Path;
        }


    //----------<Function which is executed when publish button is clicked> ---------------------------
    private void publishButton_Click(object sender, RoutedEventArgs e)
    {
            TreeViewItem t = (TreeViewItem)CodeFiles.SelectedItem;
            if (t.Items.Count == 0)
            {
                MessageBox.Show("Please select directory in Server Code Repo.");
            }
            else
            {
                string path = getParentPath(CodeFiles);
                if (path != string.Empty)
                {
                    path = path.TrimEnd('\\');
                    string msg = "Command:Publish\nPath:" + path + "\n";
                    Debug.WriteLine(msg);
                    shim.PostMessage(msg);
                }
                string msg_ = "Command:GetPublishCode\n";
                shim.PostMessage(msg_);
            }
      }

    //----------<Function which is executed when Referesh Right button is clicked> ---------------------------
    private void RefButton2_Click(object sender, RoutedEventArgs e)
    {
        string msg = "Command:GetPublishCode\n";
        shim.PostMessage(msg);

    }

    //----------<Function to build treeview from XML DOC> ---------------------------
    private void BuildTree(TreeView treeView, XDocument doc)
    {
        Debug.WriteLine(doc.Root.Name.LocalName.ToString());

        TreeViewItem treeNode = new TreeViewItem
        {
            //Should be Root
            Header = doc.Root.Attributes().First(s => s.Name == "name").Value,
            IsExpanded = true,
            IsSelected = true
        };
        if(treeView.Equals(PublishFiles))
        {
            treeNode.IsSelected = false;
            treeNode.Focusable = false;
        }
        treeView.Items.Add(treeNode);
        BuildNodes(treeNode, doc.Root, treeView);
    }

    //----------<Function which is executed when Create Folder button is clicked> ---------------------------
    private void createFolderButton_Click(object sender, RoutedEventArgs e)
    {
            TreeViewItem T = (TreeViewItem)CodeFiles.SelectedItem;
            if (T.Items.Count == 0)
                MessageBox.Show("Please select folder in Server Source Code Repo. New Folder will be created under it.");
            else
            {
                string path = getParentPath(CodeFiles);
                if (path != string.Empty)
                {
                    path += FolderNameTxtBox.Text + "\\";
                    string msg = "Command:CreateFolder\nServerPath:" + path + "\n";
                    Debug.WriteLine(msg);
                    shim.PostMessage(msg);
                }
                string msg_ = "Command:GetSourceCode\n";
                shim.PostMessage(msg_);
            }
        }

    //----------<Helper Function for Build Tree function> ---------------------------
    private void BuildNodes(TreeViewItem treeNode, XElement element, TreeView treeView)
    {
        foreach (XNode child in element.Nodes())
        {
            switch (child.NodeType)
            {
                case XmlNodeType.Element:
                    XElement childElement = child as XElement;
                    TreeViewItem childTreeNode;
                    childTreeNode = new TreeViewItem
                    {
                        //Get First attribute where it is equal to value
                        Header = childElement.Attributes().First(s => s.Name == "name").Value,
                        //Automatically expand elements
                        IsExpanded = true
                    };
                    if(childElement.Name.LocalName == "dir" && treeView.Equals(PublishFiles))
                    {
                            childTreeNode.Focusable = false;
                    }
                    treeNode.Items.Add(childTreeNode);
                    BuildNodes(childTreeNode, childElement,treeView);
                    break;
                case XmlNodeType.Text:
                    XText childText = child as XText;
                    treeNode.Items.Add(new TreeViewItem { Header = childText.Value,Focusable=false});
                    break;
            }
        }
        }

    //----------<Function to list File System Directory structure in Tree View> ---------------------------
    private static void ListDirectoryTreeView(TreeView treeView, string path)
    {
        treeView.Items.Clear();

        var rootDirectory = new DirectoryInfo(path);
       
        treeView.Items.Add(createDirectoryNode(rootDirectory));


        }

    //----------<Helper Function for ListDirectoryTreeView function> ---------------------------
    private static TreeViewItem  createDirectoryNode(DirectoryInfo directoryinfo)
    {
            var dirNode = new TreeViewItem
            {
                //Get First attribute where it is equal to value
                Header = directoryinfo.Name,
                //Automatically expand elements
                IsExpanded = true
            };
            foreach (var dir in directoryinfo.GetDirectories())
            {
                dirNode.Items.Add(createDirectoryNode(dir));
            }
            
            foreach( var files in directoryinfo.GetFiles())
            {
                var fNode = new TreeViewItem { Header = files.Name };
                dirNode.Items.Add(fNode);
            }
            return dirNode;
        }

    //----------<Function to handle incoming message from server> ---------------------------
    void MessageHandler(String msg)
    {
            if (msg.Contains("Command:GetSourceCode<dir name=\"serverSourceCodeRepo\">"))
            {
                msg = msg.Remove(0, "Command:GetSourceCode".Length);
                CodeFiles.Items.Clear();
                BuildTree(CodeFiles, XDocument.Parse(msg));
            } else if (msg.Contains("Command:GetPublishCode<dir name=\"Repository\">"))
            {
                msg = msg.Remove(0, "Command:GetPublishCode".Length);
                PublishFiles.Items.Clear();
                BuildTree(PublishFiles, XDocument.Parse(msg));
            }
            else if (msg.Contains("Fail to Publish Code"))
            {
                MessageBox.Show(msg);
            }
            else if (msg.Contains("Still Downloading"))
            {
                shim.PostMessage("Still Downloading\n");
            }
            else if (msg.Contains("Download Complete"))
            {
                string[] msg_ = msg.Split('\n');
                Process proc = new Process();
                proc.StartInfo.UseShellExecute = true;
                proc.StartInfo.FileName = msg_[1].Substring(5)+ msg_[2].Substring(5);
                proc.Start();
                string[] dirpath = msg_[1].Substring(5).Split('\\');
                string dir=string.Empty;
                for(int i=0;i < dirpath.Length-2; i++)
                {
                    dir += dirpath[i]+"\\";
                }
                ListDirectoryTreeView(PublishFiles_Client, dir);
             }
        }

    //----------<Function which is executed after GUI is loaded> ---------------------------
    private void Window_Loaded(object sender, RoutedEventArgs e)
    {
      tRcv = new Thread(
        () =>
        {
          while(true)
          {
            String msg = shim.GetMessage();
            //MessageBox.Show(msg);
            Action<String> act = new Action<string>(MessageHandler);
            Object[] args = { msg };
            Dispatcher.Invoke(act, args);
          }
        }
      );
      tRcv.Start();
      string msg_ = "Command:GetSourceCode\n";
      shim.PostMessage(msg_);
      msg_ = "Command:GetPublishCode\n";
      shim.PostMessage(msg_);
        }


    //----------<Function which is executed when Remove Button is Clicked> ---------------------------
    private void removeFile_click(object sender, RoutedEventArgs e)
    { TreeViewItem T = (TreeViewItem)CodeFiles.SelectedItem;
            if (T.Items.Count != 0)
                MessageBox.Show("Please select file you want to delete. Do not select directory");
            else {
                string path = getParentPath(CodeFiles);
                if (path != string.Empty)
                {
                    string msg = "Command:RemoveFile\nServerPath:" + path + "\n";
                    Debug.WriteLine(msg);
                    shim.PostMessage(msg);
                }
                string msg_ = "Command:GetSourceCode\n";
                shim.PostMessage(msg_);
            }
    }
    }


}
