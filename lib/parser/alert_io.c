/*
 * Licensed to Selene developers ('Selene') under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * Selene licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "parser.h"
#include "alert_messages.h"
#include "sln_tok.h"

static selene_error_t *
sln_io_alert(selene_t *s, sln_alert_level_e level, sln_alert_description_e desc)
{
  sln_bucket_t *btls = NULL;
  sln_bucket_t *balert = NULL;
  sln_msg_alert_t alert;
  sln_msg_tls_t tls;

  alert.level =  level;
  alert.description = desc;

  SELENE_ERR(sln_alert_unparse(s, &alert, &balert));

  tls.content_type = SLN_CONTENT_TYPE_ALERT;
  tls.version_major = 3;
  tls.version_minor = 1;
  tls.length = balert->size;

  SELENE_ERR(sln_tls_unparse_header(s, &tls, &btls));

  SLN_BRIGADE_INSERT_TAIL(s->bb.out_enc, btls);

  SLN_BRIGADE_INSERT_TAIL(s->bb.out_enc, balert);

  return SELENE_SUCCESS;
}

selene_error_t *
sln_io_alert_fatal(selene_t *s, sln_alert_description_e desc)
{
  return sln_io_alert(s, SLN_ALERT_LEVEL_FATAL, desc);
}

selene_error_t *
sln_io_alert_warning(selene_t *s, sln_alert_description_e desc)
{
  return sln_io_alert(s, SLN_ALERT_LEVEL_WARNING, desc);
}


selene_error_t*
sln_io_alert_read(selene_t *s, sln_parser_baton_t *baton)
{
  sln_alert_baton_t ab;

  ab.s = s;
  ab.baton = baton;
  ab.state = SLN_ALERT_STATE__INIT;
  ab.alert = sln_calloc(s, sizeof(sln_msg_alert_t));

  sln_tok_parser(baton->in_alert, sln_alert_parse, &ab);

  if (ab.state == SLN_ALERT_STATE__DONE) {
    /* TODO: emit event for alert */
    /* TODO: handle close notify */
  }

  sln_free(s, ab.alert);

  return SELENE_SUCCESS;
}
