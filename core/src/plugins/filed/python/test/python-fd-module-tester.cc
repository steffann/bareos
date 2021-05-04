/*
   BAREOS® - Backup Archiving REcovery Open Sourced

   Copyright (C) 2020-2021 Bareos GmbH & Co. KG

   This program is Free Software; you can redistribute it and/or
   modify it under the terms of version three of the GNU Affero General Public
   License as published by the Free Software Foundation, which is
   listed in the file LICENSE.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/

/* Load the python-fd extension module and test it */

#if defined(HAVE_MINGW)
#  include "include/bareos.h"
#endif


#include <string>
#include "Python.h"
#include "plugins/include/python3compat.h"
class PoolMem;
#include "filed/filed.h"
#include "lib/plugins.h"
#include "filed/fd_plugins.h"
#include "../module/bareosfd.h"

#include "../python-fd.h"

#define NbytesForBits(n) ((((n)-1) >> 3) + 1)

using namespace filedaemon;

// fd_plugins.cc
// Bareos private context
struct b_plugin_ctx {
  JobControlRecord* jcr; /* jcr for plugin */
  bRC ret;               /* last return code */
  bool disabled;         /* set if plugin disabled */
  bool restoreFileStarted;
  bool createFileCalled;
  char events[NbytesForBits(FD_NR_EVENTS + 1)]; /* enabled events bitmask */
  findIncludeExcludeItem* exclude;              /* pointer to exclude files */
  findIncludeExcludeItem* include; /* pointer to include/exclude files */
  Plugin* plugin; /* pointer to plugin of which this is an instance off */
};


// fd_plugins.cc
// Instantiate a new plugin instance.
static inline PluginContext* instantiate_plugin(JobControlRecord* jcr,
                                                Plugin* plugin,
                                                char instance)
{
  PluginContext* ctx;
  b_plugin_ctx* b_ctx;

  b_ctx = (b_plugin_ctx*)malloc(sizeof(b_plugin_ctx));
  b_ctx = (b_plugin_ctx*)memset(b_ctx, 0, sizeof(b_plugin_ctx));
  b_ctx->jcr = jcr;
  b_ctx->plugin = plugin;

  ctx = (PluginContext*)malloc(sizeof(PluginContext));
  ctx->instance = instance;
  ctx->plugin = plugin;
  ctx->core_private_context = (void*)b_ctx;
  ctx->plugin_private_context = NULL;

  if (jcr) { jcr->plugin_ctx_list->append(ctx); }

  if (PlugFunc(plugin)->newPlugin(ctx) != bRC_OK) {
    b_ctx->disabled = true;
    printf("plugin disabled\n");
  }

  return ctx;
}

bRC bareosRegisterEvents(PluginContext* ctx, int nr_events, ...)
{
  return bRC_OK;
};
bRC bareosUnRegisterEvents(PluginContext* ctx, int nr_events, ...)
{
  return bRC_OK;
};
bRC bareosGetInstanceCount(PluginContext* ctx, int* ret) { return bRC_OK; };
bRC bareosGetValue(PluginContext* ctx, filedaemon::bVariable var, void* value)
{
  return bRC_OK;
};
bRC bareosSetValue(PluginContext* ctx, filedaemon::bVariable var, void* value)
{
  return bRC_OK;
};
bRC bareosJobMsg(PluginContext* ctx,
                 const char* file,
                 int line,
                 int type,
                 utime_t mtime,
                 const char* fmt,
                 ...)
{
  va_list arg_ptr;

  printf("job(%d) %ld ", type, (int64_t)mtime);
  va_start(arg_ptr, fmt);
  vprintf(fmt, arg_ptr);
  va_end(arg_ptr);
  printf(" [%s:%d]\n", file, line);

  return bRC_OK;
};

bRC bareosDebugMsg(PluginContext* ctx,
                   const char* file,
                   int line,
                   int level,
                   const char* fmt,
                   ...)
{
  va_list arg_ptr;

  printf("debug(%d) ", level);
  va_start(arg_ptr, fmt);
  vprintf(fmt, arg_ptr);
  va_end(arg_ptr);
  printf(" [%s:%d]\n", file, line);

  return bRC_OK;
}

void* bareosMalloc(PluginContext* ctx, const char* file, int line, size_t size)
{
  return NULL;
};
void bareosFree(PluginContext* ctx, const char* file, int line, void* mem)
{
  return;
};
bRC bareosAddExclude(PluginContext* ctx, const char* file) { return bRC_OK; };
bRC bareosAddInclude(PluginContext* ctx, const char* file) { return bRC_OK; };
bRC bareosAddOptions(PluginContext* ctx, const char* opts) { return bRC_OK; };
bRC bareosAddRegex(PluginContext* ctx, const char* item, int type)
{
  return bRC_OK;
};
bRC bareosAddWild(PluginContext* ctx, const char* item, int type)
{
  return bRC_OK;
};
bRC bareosNewOptions(PluginContext* ctx) { return bRC_OK; };
bRC bareosNewInclude(PluginContext* ctx) { return bRC_OK; };
bRC bareosNewPreInclude(PluginContext* ctx) { return bRC_OK; };
bRC bareosCheckChanges(PluginContext* ctx, struct filedaemon::save_pkt* sp)
{
  return bRC_OK;
};
bRC bareosAcceptFile(PluginContext* ctx, struct filedaemon::save_pkt* sp)
{
  return bRC_OK;
}; /* Need fname and statp */
bRC bareosSetSeenBitmap(PluginContext* ctx, bool all, char* fname)
{
  return bRC_OK;
};
bRC bareosClearSeenBitmap(PluginContext* ctx, bool all, char* fname)
{
  return bRC_OK;
};

// fd_plugins.cc
/* Pointers to Bareos functions */
/* Bareos entry points */
static filedaemon::CoreFunctions bareos_core_functions
    = {sizeof(filedaemon::CoreFunctions),
       FD_PLUGIN_INTERFACE_VERSION,
       bareosRegisterEvents,
       bareosUnRegisterEvents,
       bareosGetInstanceCount,
       bareosGetValue,
       bareosSetValue,
       bareosJobMsg,
       bareosDebugMsg,
       bareosMalloc,
       bareosFree,
       bareosAddExclude,
       bareosAddInclude,
       bareosAddOptions,
       bareosAddRegex,
       bareosAddWild,
       bareosNewOptions,
       bareosNewInclude,
       bareosNewPreInclude,
       bareosCheckChanges,
       bareosAcceptFile,
       bareosSetSeenBitmap,
       bareosClearSeenBitmap};


int main(int argc, char* argv[])
{
  // disable buffering stdout
  setbuf(stdout, NULL);

  int rc = 1;

  std::string plugin_definition(
      "python"
      ":module_name=bareosfd-module-test"
      ":key1=value1:key2=value2...");
  if (argc > 1) { plugin_definition.assign(argv[1]); }

  PluginInformation* plugin_information = nullptr;
  PluginFunctions* plugin_functions = nullptr;
  PluginContext* ctx = nullptr;
  PoolMem plugin_options;

  // fd_plugins.cc
  /* Bareos info */
  static PluginApiDefinition lbareos_plugin_interface_version
      = {sizeof(PluginApiDefinition), FD_PLUGIN_INTERFACE_VERSION};

  if (loadPlugin(
          &lbareos_plugin_interface_version, &bareos_core_functions,
          &plugin_information,  // PluginInformation** plugin_information,
          &plugin_functions     // PluginFunctions** plugin_functions
          )
      != bRC_OK) {
    printf("loadPlugin failed\n");
    goto bail_out;
  }

  static Plugin plugin = {(char*)"python-fd-module-tester",  // filename
                          0,                                 // file_len
                          NULL,                              // unload
                          plugin_information,
                          plugin_functions,
                          NULL};  // plugin_handle

  ctx = instantiate_plugin(nullptr, &plugin, 0);

  if (parse_plugin_definition(ctx, plugin_definition.c_str(), plugin_options)
      != bRC_OK) {
    printf("parse_plugin_definition failed\n");
    goto bail_out;
  }
  printf("plugin_options: %s\n", plugin_options.c_str());

  PyRestoreThread();
  if (PyLoadModule(ctx, plugin_options.c_str()) != bRC_OK) {
    printf("PyLoadModule failed\n");
    goto bail_out;
  }
  printf("PyLoadModule loaded\n");

  /*
  if (Bareosfd_PyParsePluginDefinition(ctx, plugin_options.c_str()) != bRC_OK) {
    printf("Bareosfd_PyParsePluginDefinition failed\n");
    goto bail_out;
  }
  */

  // plugin.unloadPlugin();
  // PySaveThread();
  // Py_Finalize();
  rc = 0;

bail_out:
  return rc;
}
