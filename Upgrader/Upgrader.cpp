
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <Alert.h>
#include <Application.h>
#include <Message.h>
#include <Roster.h>
#include <StatusBar.h>
#include <String.h>
#include <TextView.h>
#include <Window.h>
#include <Path.h>
#include <Roster.h>

#include <fs_attr.h>

#include <NetAddress.h>
#include <NetEndpoint.h>

#include "Upgrader.h"

extern "C" _IMPEXP_ROOT int _kset_fd_limit_(int num);


// Dano's BBox is *ugly* in B_FANCY_BORDER mode (plus there's really nothing fancy about a single-color
// rounded rectangle...), so let's make our own.
class FancyBox: public BView
{
	public:
		FancyBox(): BView(BRect(0,0,10,10),"",B_WILL_DRAW,B_FOLLOW_ALL_SIDES)
		{
		}

		virtual void Draw(BRect)
		{
			BRect b = Bounds();
			SetHighColor(255,255,255,255);
			StrokeLine(b.LeftTop(),b.RightTop());
			StrokeLine(b.LeftTop(),b.LeftBottom());
			SetHighColor(64,64,64,255);
			StrokeLine(b.RightBottom(),b.RightTop());
			StrokeLine(b.RightBottom(),b.LeftBottom());
		}

		virtual void AttachedToWindow()
		{
			BRect pb = Window()->Bounds();
			ResizeTo(pb.Width(),pb.Height());
			SetViewColor(216,216,216);
		}
};


class ProgressWindow: public BWindow
{
	private:
		BStatusBar *bar;
		bool quitwindow;
		FancyBox *box;
		int32 totalsize;
		bigtime_t starttime;
		float elapsedcounter;

	public:
		ProgressWindow()
			: BWindow(BRect(100,100,300,200),"Contacting server"B_UTF8_ELLIPSIS, B_TITLED_WINDOW,B_NOT_RESIZABLE)
		{
			quitwindow = false;
			bar = NULL;
			totalsize = 0;
			box = new FancyBox();
			AddChild(box);
			Show();
		}

		void SetFileLength(int32 size)
		{
			char trail[10];
			totalsize = size;
			float kbytes = (float) size/1024;
			if(kbytes<1000)
				sprintf(trail,"%.1fK",kbytes);
			else
				sprintf(trail,"%.1fM",kbytes/1024);

			bar = new BStatusBar(Bounds().InsetBySelf(5,5),"","", trail);

			box->AddChild(bar);
			bar->SetMaxValue(size);
			SetTitle("Downloading"B_UTF8_ELLIPSIS);
			starttime = system_time();
			elapsedcounter = 1.0;
		}

		virtual void MessageReceived(BMessage *mes)
		{
			switch(mes->what)
			{
				case 'prog':
					if(totalsize)
					{
						int32 amount = mes->FindInt32("amount");
						char label[10];
						float curvalue = bar->CurrentValue();
						sprintf(label,"%ld%%", int32((curvalue+amount)*100/bar->MaxValue()));
						bar->Update(amount, label);

//						bigtime_t now = system_time();
//						float elapsed = float(now - starttime)/1000000;
//						if(elapsed > elapsedcounter)
//						{
//							while(elapsed > elapsedcounter)
//								elapsedcounter += 1.0;
//							float speed = curvalue / elapsed; // bytes/second
//							float left = totalsize - curvalue;
//							int32 timeleft = int32(left / speed);
//							printf("time left: %d, %d, %d, %d, %d\n",int32(curvalue), totalsize,
//																int32(elapsed), int32(speed), timeleft);
//						}
					}
					break;

				default:
					break;
			}
		}

		virtual bool QuitRequested()
		{
			return true;//quitwindow;
		}
};


static BString FindBlock(BString &data, const char *blockname)
{
	BString name;

	name << "[" << blockname << "]\n";

	int32 offset = data.FindFirst(name);
	if(offset<0)
		return "";
	offset += name.CountChars();

	int32 nextoffset = data.FindFirst("\n\n[", offset);
	if(nextoffset < 0)
		nextoffset = data.CountChars();

	BString block;
	data.CopyInto(block, offset, nextoffset-offset);
	return block;
}


static BString RemoveFirstLine(BString& string)
{
	BString line;
	int32 offset = string.FindFirst("\n");
	if(offset>=0)
	{
		string.CopyInto(line, 0 , offset);
		string.Remove(0,offset+1);
	}
	return line;
}

static status_t ReadData(const char *url, const char *filepath, bool showprogress = false)
{
	if(strncasecmp(url,"http://",7)==0)
		url+=7;

	char host[1000];
	strcpy(host,url);
	int32 port = 80;
	char *portstring = strstr(host,":");
	if(portstring)
	{
		port = atoi(portstring+1);
		*portstring = 0;
	}

	char *file="";
	char *filestring = strstr(host,"/");
	if(filestring)
	{
		*filestring=0;
		file=filestring+1;
	}

	BNetAddress address(0x0100007f,0);
	address.SetTo(host,port);
	if(B_OK != address.InitCheck())
	{
		//fprintf(stderr,"Couldn't get network address for %s\n", host);
		address.SetTo(uint32(0),int(0));
		return 2;
	}

	BNetEndpoint *endpoint = new BNetEndpoint();
	status_t status;

	status = endpoint->InitCheck();
	if(B_OK != status)
	{
		//fprintf(stderr,"Couldn't create socket: %08x\n",status);
		return 3;
	}


	// ready to start downloading
	ProgressWindow *win = NULL;
	if(showprogress)
		win = new ProgressWindow();

	status = endpoint->Connect(address);
	if(B_OK != status)
	{
		//fprintf(stderr,"Couldn't connect socket: %08x\n",status);
		if(win->Lock()) win->Quit();
		return 4;
	}

	endpoint->SetTimeout(10000000);	// 10 second timeout

	BString string;
	string << "GET /" << file << " HTTP/1.0\r\n";
	string << "User-Agent: Upgrader/BeOS\r\n";
	string << "\r\n";
//	int32 numwritten = 
		endpoint->Send(string.String(),string.Length());
//	if(numwritten<0)
//		fprintf(stderr,"Error: %s\n",endpoint->ErrorStr());

	//fprintf(stderr,"Requests sent\n");

	BString response;
	char c;
	for(int i=0;i<100000;i++)
	{
		if(endpoint->Receive(&c,1)<=0)
			break;
		response << c;
		response.ReplaceFirst("\r\n","\n");
		if(response.FindFirst("\n\n")>0)
		{
			BString line = RemoveFirstLine(response);

			int32 httpstatus;
			if(1 != sscanf(line.String(),"HTTP/%*s %d %*s", &httpstatus))
			{
				//fprintf(stderr,"Bad server response: %s\n",line.String());
				if(win->Lock()) win->Quit();
				return 5;
			}

			if(httpstatus == 301 || httpstatus == 302)
			{
				// redirect
				BString location;
				while(true)
				{
					location = RemoveFirstLine(response);
					if(line == "")
						break;		// unknown length
					if(location.IFindFirst("Location:")==0)
					{
						location.Remove(0,9);
						if(location.FindFirst(" ")==0)
							location.Remove(0,1);
						if(win->Lock()) win->Quit();
						return ReadData(location.String(), filepath, showprogress);
					}
				}
				// fall through
			}

			if(httpstatus != 200)
			{
				//fprintf(stderr,"Server error: %s\n",line.String());
				if(win->Lock()) win->Quit();
				return 6;
			}

			//write body of data to file
			int outputfile = open(filepath, O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR);
			if(outputfile == -1)
			{
				//fprintf(stderr,"Couldn't open tmp file\n");
				if(win->Lock()) win->Quit();
				return 7;
			}

			int32 filelength = -1;
			while(true)
			{
				line = RemoveFirstLine(response);
				if(line == "")
					break;		// unknown length
				if(line.IFindFirst("Content-Length:")==0)
				{
					if(1 != sscanf(line.String(),"%*s %d",&filelength))
					{
						filelength = -1;
					}
				}
				if(line.IFindFirst("Content-Type:")==0)
				{
					char filetype[101];
					if(1 == sscanf(line.String(),"%*s %100s",filetype))
					{
						fs_write_attr(outputfile,"BEOS:TYPE",B_MIME_TYPE,0,filetype,strlen(filetype)+1);
					}
				}
			}

			char buf[10000];
			int32 readsize = 4000;
			if(filelength>0 && win)
			{
				win->SetFileLength(filelength);
				readsize = min_c(10000, max_c(1000, filelength/200));
			}
			while(true)
			{
				int numread = endpoint->Receive(buf,readsize);
				if(numread<=0)
					break;

				if(win)
				{
					BMessage msg('prog');
					msg.AddInt32("amount",numread);
					if(win->PostMessage(&msg)!=B_OK)
					{
						//fprintf(stderr,"User aborted download\n");
						if(win->Lock()) win->Quit();
						return 8;
					}
				}

				int numwritten = write(outputfile,buf,numread);
				if(numwritten!=numread)
				{
					//fprintf(stderr,"Couldn't write tmp file\n");
					if(win->Lock()) win->Quit();
					return 9;
				}
			}
			close(outputfile);
			if(win->Lock()) win->Quit();
			return 0;
		}
	}
	//fprintf(stderr,"Error retrieving file\n");
	if(win->Lock()) win->Quit();
	return 10;
}

static void wait_for_team(team_id teamid)
{
	team_info info;
	while(B_OK == get_team_info(teamid, &info))
		snooze(500000);
}

static const char *doupgrade(const char *currentversion, const char *updateurl);

void DoUpgrade(const char *currentversion, const char *updateurl)
{
	const char *err = doupgrade(currentversion,updateurl);
	if(err)
	{
		BAlert *alert = new BAlert("Update", err, "OK");
		alert->Go();
	}
}


static const char *doupgrade(const char *currentversion, const char *updateurl)
{
	signal(SIGPIPE,SIG_IGN);

	BString infoname;
	infoname << "/tmp/upgradeinfo" << find_thread(NULL);

	if(ReadData(updateurl, infoname.String()))
		return "Couldn't retrieve update script";

	// read upgrade-info into string and delete file
	BString upgrade;
	BFile file(infoname.String(), B_READ_ONLY);
	char c;
	while(1 == file.Read(&c,1))
		upgrade << c;
	file.Unset();
	unlink(infoname.String());

	upgrade.ReplaceAll("\r\n","\n");

	int32 bodyoffset = upgrade.FindFirst("\n\n");
	if(bodyoffset >= 0)
		upgrade.Remove(0,bodyoffset+2);

	if(upgrade == "")
	{
		//fprintf(stderr,"Error: empty document\n");
		return "Update script was empty!";
	}

	/* -----------------------------------------------------------
		The 'upgrade' string now contains the retrieved document.
		Parse its content.
	 ----------------------------------------------------------- */

	BString version = FindBlock(upgrade,"version");

	if(strcmp(version.String(),currentversion)<=0)
		return "No update is currently available.";

	BString message = FindBlock(upgrade,"message");

	BString webpage = FindBlock(upgrade,"webpage");
	bool haswebpage = (webpage!="");

	const char *button1 = "Cancel";
	const char *button2 = haswebpage?"More info"B_UTF8_ELLIPSIS:"Upgrade!";
	const char *button3 = haswebpage?"Upgrade!":NULL;

	BAlert *alert = new BAlert("Upgrade available", message.String(), button1, button2, button3);
	int32 choice = alert->Go();

	if(haswebpage && choice == 1)
	{
		const char *url = webpage.String();
		be_roster->Launch("text/html", 1, (char**)&url);
		return NULL;
	}
	else if(choice == 0)
	{
		// do nothing
		return NULL;
	}

	//fprintf(stderr, "Upgrading...\n");

	// download file
	BString fileurl = FindBlock(upgrade,"file");
	BString filename;
	filename << "/tmp/upgradefile" << find_thread(NULL);
	if(ReadData(fileurl.String(),filename.String(), true))
	{
		unlink(filename.String());
		return "Couldn't download upgrade file!";
	}

	// run script
	BString execute = FindBlock(upgrade,"execute");

	if(execute == "")
	{
		//fprintf(stderr,"no script, Launch()ing\n");
		// BRoster::Launch() the file (most likely this will launch SoftwareValet or Expander)
		entry_ref ref;
		get_ref_for_path(filename.String(),&ref);
		team_id teamid;
		be_roster->Launch(&ref, (BMessage*)NULL, &teamid);
		if(teamid<0)
			return "Couldn't launch downloaded file!\n(Downloaded file is in /tmp)";

		wait_for_team(teamid);
		unlink(filename.String());
		return NULL;
	}
	
	BString scriptname;
	scriptname << "/tmp/upgradescript" << find_thread(NULL);
	BFile script(scriptname.String(),B_CREATE_FILE|B_WRITE_ONLY);
	if(script.InitCheck()!=B_OK)
		return "Couldn't execute upgrade script!\n(Downloaded file is in /tmp)";

	script.Write(execute.String(),execute.Length());
	script.SetPermissions(S_IRUSR|S_IXUSR);
	script.Unset();

	BPath path;
	app_info appinfo;
	be_app->GetAppInfo(&appinfo);
	BEntry entry(&appinfo.ref);
	entry.GetParent(&entry);
	entry.GetPath(&path);

	const char *script_argv[] = {path.Path(), filename.String()};
	int32 script_argc = sizeof(script_argv)/sizeof(char*);
	entry_ref ref;
	get_ref_for_path(scriptname.String(),&ref);
	team_id teamid;
	be_roster->Launch(&ref, script_argc, script_argv, &teamid);
	if(teamid<0)
	{
		unlink(scriptname.String());
		return "Couldn't execute upgrade script!\n(Downloaded file is in /tmp)";
	}
	wait_for_team(teamid);
	unlink(scriptname.String());
	return NULL;
}
