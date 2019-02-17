
#include <Application.h>
#include <Alert.h>
#include <Roster.h>
#include <String.h>
#include <Path.h>
#include <Directory.h>
#include <Entry.h>
#include <List.h>

#include <stdio.h>
#include <malloc.h>

static char *MakePathRelative(const char *base, const char *subpath);
static const char *GetFileSpec(const char *file_and_path);
static char *RemoveFileSpec(const char *input_file);
static int GetDirectoryContents(const char *directory, BList *list_out, BList *isdir_out);
static void FreeBList(BList *list);

static BList gFileList;
static BList gIsDirList;

#define APPLICATION_SIGNATURE	"application/xvnd-Kt.CloneKeen"
/*
void c------------------------------() {}
*/

int main(int argc, char *argv[])
{
	// set current directory to that of our executable, so we will work from Tracker
	{
		BApplication app(APPLICATION_SIGNATURE);
		app_info info;
		BPath myPath;
		
		app.GetAppInfo(&info);
		BEntry entry(&info.ref);
		entry.GetPath(&myPath);
		myPath.GetParent(&myPath);
		
		const char *path = myPath.Path();
		if (path)
			chdir(path);
	}
	
	return KeenMain(argc, argv);
}

void platform_msgbox(const char *message)
{
BApplication *app = NULL;

	// we must have a BApplication to open a BAlert...
	// SDL will create one for us, if it hasn't been started yet
	// then we will have to.
	if (be_app == NULL)
		app = new BApplication(APPLICATION_SIGNATURE);
	
	(new BAlert("", message, "OK"))->Go();
	
	if (app)
	{
		delete app;
		be_app = NULL;
	}
}

/*
void c------------------------------() {}
*/

int LoadDirectoryListing(char *path)
{
	// make path relative to base
	BRoster roster;
	app_info info;
	BPath myPath;
	
	roster.GetRunningAppInfo(find_thread(0), &info);
	BEntry entry(&info.ref);
    entry.GetPath(&myPath);

	char *fullpath = MakePathRelative(myPath.Path(), path);
	
	// now load directory contents
	GetDirectoryContents(fullpath, &gFileList, &gIsDirList);
	return gFileList.CountItems();
}

const char *GetFileAtIndex(int fno)
{
	return (const char *)gFileList.ItemAt(fno);
}

char GetIsDirectory(int fno)
{
	return (char)gIsDirList.ItemAt(fno);
}

void FreeDirectoryListing(void)
{
	FreeBList(&gFileList);
	gIsDirList.MakeEmpty();
}
	
	

/*
void c------------------------------() {}
*/
	
// makes "filename" relative to file "relative_to", and returns the fixed up path.
// if chroot is true, then even absolute paths are "concated" to relative_to.
static char *MakePathRelative(const char *base, const char *subpath)
{
	BString fullpath("", MAXPATHLEN);
	
	// if it isn't an absolute path, we need to make it one
	if (subpath[0] && subpath[0] != '/')
	{
		if (subpath[0] == '/') subpath++;	// for chrooting
		
		// get current path, and copy into filename string
		char *relative_path = RemoveFileSpec(base);
		fullpath.SetTo(relative_path);
		free(relative_path);
		
		// append trailing '/' if missing
		int len = fullpath.Length();
		if (len && fullpath.ByteAt(len-1) != '/')
			fullpath.Append("/");
	}
	
	// append the relative filename to the path, then canonicalize
	fullpath.Append(subpath);
	BPath path(fullpath.String());
	const char *pathstr = path.Path();
	return strdup(pathstr ? pathstr : fullpath.String());
}

// given a full path to a file, return onto the name of the file.
// the pointer returned is within the original string.
static const char *GetFileSpec(const char *file_and_path)
{
	char *ptr = strrchr(file_and_path, '/');
	if (ptr)
		return ptr+1;
	else
		return file_and_path;
}

// given a full path to a file, return the path part without the filename.
// the pointer returned is an allocated area of memory which you need to frees().
static char *RemoveFileSpec(const char *input_file)
{
	char *buffer = strdup(input_file);
	char *ptr = strrchr(buffer, '/');
	if (ptr) *(ptr + 1) = 0;
	return buffer;
}

/*
void c------------------------------() {}
*/

// fills "dirs" and "files" with char * strings containing the names of all directories
// and all files matching a given filter within the given folder.
// returns nonzero if an error occurs.
static int GetDirectoryContents(const char *directory, BList *list_out, BList *isdir_out)
{
	FreeBList(list_out);
	isdir_out->MakeEmpty();
	
	BEntry entry(directory);
	if (!entry.Exists())
	{
		lprintf("haiku GetDirectoryContents: cannot open directory %s\n", directory);
		return 1;
	}
	
	BDirectory dirchecker;
	
	BDirectory dir(&entry);
	dir.Rewind();
	
	while(dir.GetNextEntry(&entry, false) != B_ENTRY_NOT_FOUND)
	{
		BPath path;
		entry.GetPath(&path);
		
		const char *pathname = path.Path();
		
		if (pathname)
		{
			list_out->AddItem(strdup(GetFileSpec(pathname)));
			
			// check if this is a file or a directory. to do that, i just pretend it's
			// a directory and see if that assumption causes an error.
			dirchecker.SetTo(&entry);
			if (dirchecker.Rewind() != B_OK)
			{	// is file
				isdir_out->AddItem((void *)0);
			}
			else
			{	// is directory
				isdir_out->AddItem((void *)1);
			}
		}
	}
	
	return 0;
}

static void FreeBList(BList *list)
{
	int i, count = list->CountItems();
	for(i=0;i<count;i++)
		free(list->ItemAt(i));
	list->MakeEmpty();
}





