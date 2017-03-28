// -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#include <iostream>

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "include/types.h"
#include "include/utime.h"
#include "objclass/objclass.h"

#include "cls_log_types.h"
#include "cls_log_ops.h"

#include "global/global_context.h"
#include "include/compat.h"

//Yuanguo: add log <<<<<<<<<<<<<<<<<<<
#include "osd/ReplicatedPG.h"
#include "osd/osd_types.h"
//Yuanguo: finished adding log  >>>>>>>>>>>>>>

CLS_VER(1,0)
CLS_NAME(log)

cls_handle_t h_class;
cls_method_handle_t h_log_add;
cls_method_handle_t h_log_list;
cls_method_handle_t h_log_trim;
cls_method_handle_t h_log_info;

static string log_index_prefix = "1_";


static int write_log_entry(cls_method_context_t hctx, string& index, cls_log_entry& entry)
{

  //Yuanguo: add log <<<<<<<<<<<<<<<<<<<
  ReplicatedPG::OpContext **pctx = (ReplicatedPG::OpContext **)hctx;
  string oname = (*pctx)->new_obs.oi.soid.oid.name;
  CLS_LOG(99, "YuanguoDbg: write_log_entry, %s index=%s entry=[%s, %s, %s]\n", oname.c_str(), index.c_str(), entry.id.c_str(), entry.section.c_str(), entry.name.c_str());
  //Yuanguo: finished adding log  >>>>>>>>>>>>>>


  bufferlist bl;
  ::encode(entry, bl);

  int ret = cls_cxx_map_set_val(hctx, index, &bl);
  if (ret < 0)
    return ret;

  return 0;
}

static void get_index_time_prefix(utime_t& ts, string& index)
{
  char buf[32];
  snprintf(buf, sizeof(buf), "%010ld.%06ld_", (long)ts.sec(), (long)ts.usec());

  index = log_index_prefix + buf;
}

static int read_header(cls_method_context_t hctx, cls_log_header& header)
{
  bufferlist header_bl;

  int ret = cls_cxx_map_read_header(hctx, &header_bl);
  if (ret < 0)
    return ret;

  if (header_bl.length() == 0) {
    header = cls_log_header();
    return 0;
  }

  bufferlist::iterator iter = header_bl.begin();
  try {
    ::decode(header, iter);
  } catch (buffer::error& err) {
    CLS_LOG(0, "ERROR: read_header(): failed to decode header");
  }

  return 0;
}

static int write_header(cls_method_context_t hctx, cls_log_header& header)
{
  bufferlist header_bl;
  ::encode(header, header_bl);

  int ret = cls_cxx_map_write_header(hctx, &header_bl);
  if (ret < 0)
    return ret;

  return 0;
}

static void get_index(cls_method_context_t hctx, utime_t& ts, string& index)
{
  get_index_time_prefix(ts, index);

  string unique_id;

  cls_cxx_subop_version(hctx, &unique_id);

  index.append(unique_id);
}

static int cls_log_add(cls_method_context_t hctx, bufferlist *in, bufferlist *out)
{
  bufferlist::iterator in_iter = in->begin();

  cls_log_add_op op;
  try {
    ::decode(op, in_iter);
  } catch (buffer::error& err) {
    CLS_LOG(1, "ERROR: cls_log_add_op(): failed to decode op");
    return -EINVAL;
  }

  //Yuanguo: add log <<<<<<<<<<<<<<<<<<<
  ReplicatedPG::OpContext **pctx = (ReplicatedPG::OpContext **)hctx;
  string oname = (*pctx)->new_obs.oi.soid.oid.name;
  CLS_LOG(99, "YuanguoDbg: cls_log_add, %s op.monotonic_inc=%d\n", oname.c_str(), (int)op.monotonic_inc);
  for(list<cls_log_entry>::const_iterator itr=op.entries.begin(); itr!=op.entries.end(); ++itr)
  {
    CLS_LOG(1, "YuanguoDbg: cls_log_add, cls_log_entry: [%s, %s, %s]\n", itr->id.c_str(), itr->section.c_str(), itr->name.c_str());
  }
  //Yuanguo: finished adding log  >>>>>>>>>>>>>>

  cls_log_header header;

  int ret = read_header(hctx, header);
  if (ret < 0)
    return ret;

  CLS_LOG(1, "YuanguoDbg: cls_log_add, ret=%d, header.max_marker=%s\n", ret, header.max_marker.c_str());

  for (list<cls_log_entry>::iterator iter = op.entries.begin();
       iter != op.entries.end(); ++iter) {
    cls_log_entry& entry = *iter;

    CLS_LOG(1, "YuanguoDbg: cls_log_add, for cls_log_entry: [%s, %s, %s]\n", entry.id.c_str(), entry.section.c_str(), entry.name.c_str());

    string index;

    utime_t timestamp = entry.timestamp;
    if (op.monotonic_inc && timestamp < header.max_time)
      timestamp = header.max_time;
    else if (timestamp > header.max_time)
      header.max_time = timestamp;

    if (entry.id.empty()) {
      get_index(hctx, timestamp, index);
      entry.id = index;
    } else {
      index = entry.id;
    }

    CLS_LOG(0, "storing entry at %s", index.c_str());

    if (index > header.max_marker)
      header.max_marker = index;

    //Yuanguo: set omap key-value pair which can be listed by:
    //   # rados listomapkeys data_log.3  -p zone_master_hyg.rgw.log
    //   1_1489653363.684562_381.1
    ret = write_log_entry(hctx, index, entry);
    if (ret < 0)
      return ret;
  }

  CLS_LOG(1, "YuanguoDbg: cls_log_add, write_header, ret=%d header.max_marker=%s\n", ret, header.max_marker.c_str());
  //Yuanguo: set omap header which can be read by:
  //    # rados getomapheader data_log.3 ttt -p zone_master_hyg.rgw.log
  ret = write_header(hctx, header);
  if (ret < 0)
    return ret;

  return 0;
}

static int cls_log_list(cls_method_context_t hctx, bufferlist *in, bufferlist *out)
{
  bufferlist::iterator in_iter = in->begin();

  cls_log_list_op op;
  try {
    ::decode(op, in_iter);
  } catch (buffer::error& err) {
    CLS_LOG(1, "ERROR: cls_log_list_op(): failed to decode op");
    return -EINVAL;
  }

  //Yuanguo: add log <<<<<<<<<<<<<<<<<<<
  ReplicatedPG::OpContext **pctx = (ReplicatedPG::OpContext **)hctx;
  string oname = (*pctx)->new_obs.oi.soid.oid.name;
  CLS_LOG(99, "YuanguoDbg: cls_log_list, %s op=[%ld, %s, %ld, %d]\n", oname.c_str(), op.from_time.to_msec(), op.marker.c_str(), op.to_time.to_msec(), op.max_entries);
  //Yuanguo: finished adding log  >>>>>>>>>>>>>>

  map<string, bufferlist> keys;

  string from_index;
  string to_index;

  if (op.marker.empty()) {
    get_index_time_prefix(op.from_time, from_index);
  } else {
    from_index = op.marker;
  }
  bool use_time_boundary = (!op.from_time.is_zero() && (op.to_time >= op.from_time));

  if (use_time_boundary)
    get_index_time_prefix(op.to_time, to_index);

#define MAX_ENTRIES 1000
  size_t max_entries = op.max_entries;
  if (!max_entries || max_entries > MAX_ENTRIES)
    max_entries = MAX_ENTRIES;

  int rc = cls_cxx_map_get_vals(hctx, from_index, log_index_prefix, max_entries + 1, &keys);

  CLS_LOG(99, "YuanguoDbg: cls_log_list, rc=%d keys.size=%d\n", rc, keys.size());

  if (rc < 0)
    return rc;

  cls_log_list_ret ret;

  list<cls_log_entry>& entries = ret.entries;
  map<string, bufferlist>::iterator iter = keys.begin();

  bool done = false;
  string marker;

  size_t i;
  for (i = 0; i < max_entries && iter != keys.end(); ++i, ++iter) 
  {
    const string& index = iter->first;

    CLS_LOG(99, "YuanguoDbg: cls_log_list, index=%s\n", index.c_str());

    marker = index;
    if (use_time_boundary && index.compare(0, to_index.size(), to_index) >= 0) {
      CLS_LOG(99, "YuanguoDbg: cls_log_list, index=%s done\n", index.c_str());
      done = true;
      break;
    }

    bufferlist& bl = iter->second;
    bufferlist::iterator biter = bl.begin();
    try {
      cls_log_entry e;
      ::decode(e, biter);
      entries.push_back(e);

      CLS_LOG(99, "YuanguoDbg: cls_log_list, index=%s e=[%s, %s, %s, %ld]\n", index.c_str(), e.id.c_str(), e.section.c_str(), e.name.c_str(), e.timestamp.to_msec());

    } catch (buffer::error& err) {
      CLS_LOG(0, "ERROR: cls_log_list: could not decode entry, index=%s", index.c_str());
    }
  }

  if (iter == keys.end())
    done = true;

  ret.marker = marker;
  ret.truncated = !done;

  ::encode(ret, *out);

  return 0;
}


static int cls_log_trim(cls_method_context_t hctx, bufferlist *in, bufferlist *out)
{
  bufferlist::iterator in_iter = in->begin();

  cls_log_trim_op op;
  try {
    ::decode(op, in_iter);
  } catch (buffer::error& err) {
    CLS_LOG(0, "ERROR: cls_log_list_op(): failed to decode entry");
    return -EINVAL;
  }

  map<string, bufferlist> keys;

  string from_index;
  string to_index;

  if (op.from_marker.empty()) {
    get_index_time_prefix(op.from_time, from_index);
  } else {
    from_index = op.from_marker;
  }
  if (op.to_marker.empty()) {
    get_index_time_prefix(op.to_time, to_index);
  } else {
    to_index = op.to_marker;
  }

#define MAX_TRIM_ENTRIES 1000
  size_t max_entries = MAX_TRIM_ENTRIES;

  int rc = cls_cxx_map_get_vals(hctx, from_index, log_index_prefix, max_entries, &keys);
  if (rc < 0)
    return rc;

  map<string, bufferlist>::iterator iter = keys.begin();

  size_t i;
  bool removed = false;
  for (i = 0; i < max_entries && iter != keys.end(); ++i, ++iter) {
    const string& index = iter->first;

    CLS_LOG(20, "index=%s to_index=%s", index.c_str(), to_index.c_str());

    if (index.compare(0, to_index.size(), to_index) > 0)
      break;

    CLS_LOG(20, "removing key: index=%s", index.c_str());

    int rc = cls_cxx_map_remove_key(hctx, index);
    if (rc < 0) {
      CLS_LOG(1, "ERROR: cls_cxx_map_remove_key failed rc=%d", rc);
      return -EINVAL;
    }
    removed = true;
  }

  if (!removed)
    return -ENODATA;

  return 0;
}

static int cls_log_info(cls_method_context_t hctx, bufferlist *in, bufferlist *out)
{
  bufferlist::iterator in_iter = in->begin();

  cls_log_info_op op;
  try {
    ::decode(op, in_iter);
  } catch (buffer::error& err) {
    CLS_LOG(1, "ERROR: cls_log_add_op(): failed to decode op");
    return -EINVAL;
  }

  //Yuanguo: add log <<<<<<<<<<<<<<<<<<<
  ReplicatedPG::OpContext **pctx = (ReplicatedPG::OpContext **)hctx;
  string oname = (*pctx)->new_obs.oi.soid.oid.name;
  CLS_LOG(99, "YuanguoDbg: cls_log_info, %s op=empty\n", oname.c_str());
  //Yuanguo: finished adding log  >>>>>>>>>>>>>>

  cls_log_info_ret ret;

  int rc = read_header(hctx, ret.header);

  CLS_LOG(99, "YuanguoDbg: cls_log_info, ret.header=[%s, %ld]\n", ret.header.max_marker.c_str(), ret.header.max_time.to_msec());

  if (rc < 0)
    return rc;

  ::encode(ret, *out);

  return 0;
}

void __cls_init()
{
  CLS_LOG(1, "Loaded log class!");

  cls_register("log", &h_class);

  /* log */
  cls_register_cxx_method(h_class, "add", CLS_METHOD_RD | CLS_METHOD_WR, cls_log_add, &h_log_add);
  cls_register_cxx_method(h_class, "list", CLS_METHOD_RD, cls_log_list, &h_log_list);
  cls_register_cxx_method(h_class, "trim", CLS_METHOD_RD | CLS_METHOD_WR, cls_log_trim, &h_log_trim);
  cls_register_cxx_method(h_class, "info", CLS_METHOD_RD, cls_log_info, &h_log_info);

  return;
}

