/*
 * Initial main.c file generated by Glade. Edit as required.
 * Glade will not overwrite this file.
 */
#ifdef _DEBUG
# pragma comment (lib, "../../../import/libtkbD.lib")
#else
# pragma comment(lib, "../../../import/libtkb.lib")
#endif

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <memory.h>
#include <stdio.h>

#include "../libtkb/export/libtkb.h"
#include "interface.h"
#include "support.h"
#include "callbacks.h"
#include "tkb32clGUIParam.h"
#include "callbacksSub.h"

#ifdef G_OS_WIN32
gchar *package_prefix = PACKAGE_PREFIX;
gchar *package_data_dir = PACKAGE_DATA_DIR;
gchar *package_locale_dir = PACKAGE_LOCALE_DIR;
#else
gchar *package_prefix = "";
gchar *package_data_dir = "";
gchar *package_locale_dir = "";
#endif

int
main (int argc, char *argv[])
{
  const char dir[] = "../etc";
  GtkWidget *winTkb32clGUI;
  GtkWidget *winAllStatus;
  GtkWidget *winParameterList;
  GtkWidget *winMap;
  gchar *pixmap_dir;
  int ret;
  long size;
  char tmp[256];
  int i;
  void* p;
 
  confInit();
  sprintf(tmp, "%s/tkb32cl.conf", dir);
  if(confAddFile(tmp)){
    uM("main(); confAddFile(%s); error!!", tmp);
    return 1;
    /* throw "";*/
  }
  uInit("../log/tkb32clGUI");
  //uM2("Compile at %s %s\n", __DATE__, __TIME__); //!< �Ȃ������s���ɗ�����
  ret = tkb32clFuncInit();
  if(ret){
    uM("main(); tkb32clFuncInit(); error!!");
    return 1;
  }
  p = callbacksSubInit(argc, argv);

#ifdef G_OS_WIN32
  package_prefix = g_win32_get_package_installation_directory (NULL, NULL);
  package_data_dir = g_build_filename (package_prefix, "share", NULL);
  package_locale_dir = g_build_filename (package_prefix, "share", "locale", NULL);
#endif

#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, package_locale_dir);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);
#endif

  gtk_set_locale ();
  gtk_init (&argc, &argv);

  //pixmap_dir = g_build_filename (package_data_dir, PACKAGE, "pixmaps", NULL);
  //add_pixmap_directory (pixmap_dir);
  //g_free (pixmap_dir);


  /*
   * The following code was added by Glade to create one of each component
   * (except popup menus), just so that you see something after building
   * the project. Delete any components that you don't want shown initially.
   */
  winAllStatus = create_winAllStatus (p);
  winParameterList = create_winParameterList (p);
  winTkb32clGUI = create_winTkb32clGUI (p);
  winMap = create_winMap (p);
  callbacksSubViewParam(p);
  gtk_main ();

#ifdef G_OS_WIN32
  g_free (package_prefix);
  g_free (package_data_dir);
  g_free (package_locale_dir);
#endif

  return 0;
}
#ifdef _MSC_VER
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
  return main (__argc, __argv);
}
#endif
