/*
 * Copyright (c) 2004 Massachusetts Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* $Id$ */

#include<krbcred.h>
#include<kherror.h>
#include<khmsgtypes.h>
#include<strsafe.h>
#include<krb5.h>

#include<assert.h>

extern LPVOID k5_main_fiber;
extern LPVOID k5_kinit_fiber;

typedef struct k5_dlg_data_t {
    khui_new_creds * nc;

    khui_tracker tc_lifetime;
    khui_tracker tc_renew;

    BOOL dirty;

    DWORD   renewable;
    DWORD   forwardable;
    DWORD   proxiable;
    DWORD   addressless;
    DWORD   publicIP;

    wchar_t * cred_message;     /* overrides the credential text, if
                                   non-NULL */
} k5_dlg_data;


INT_PTR
k5_handle_wm_initdialog(HWND hwnd,
                        WPARAM wParam,
                        LPARAM lParam)
{
    HWND hw;
    k5_dlg_data * d;
    khui_new_creds_by_type * nct;
    
    d = malloc(sizeof(*d));
    ZeroMemory(d, sizeof(*d));
    /* lParam is a pointer to a khui_new_creds structure */
    d->nc = (khui_new_creds *) lParam;
    khui_cw_find_type(d->nc, credtype_id_krb5, &nct);

#pragma warning(push)
#pragma warning(disable: 4244)
    SetWindowLongPtr(hwnd, DWLP_USER, (LPARAM) d);
#pragma warning(pop)

    nct->aux = (LPARAM) d;

    if (d->nc->subtype == KMSG_CRED_NEW_CREDS) {
        khui_tracker_initialize(&d->tc_lifetime);
        khui_tracker_initialize(&d->tc_renew);

        hw = GetDlgItem(hwnd, IDC_NCK5_LIFETIME_EDIT);
        khui_tracker_install(hw, &d->tc_lifetime);

        hw = GetDlgItem(hwnd, IDC_NCK5_RENEW_EDIT);
        khui_tracker_install(hw, &d->tc_renew);
    }
    return TRUE;
}

INT_PTR
k5_handle_wm_destroy(HWND hwnd,
                     WPARAM wParam,
                     LPARAM lParam)
{
    k5_dlg_data * d;
    khui_new_creds_by_type * nct = NULL;

    d = (k5_dlg_data *) (LONG_PTR)
        GetWindowLongPtr(hwnd, DWLP_USER);
#ifdef DEBUG
    assert(d);
#endif

    khui_cw_find_type(d->nc, credtype_id_krb5, &nct);

#ifdef DEBUG
    assert(nct);
#endif

    nct->aux = 0;

    if (d->nc->subtype == KMSG_CRED_NEW_CREDS) {
        khui_tracker_kill_controls(&d->tc_renew);
        khui_tracker_kill_controls(&d->tc_lifetime);
    }

    free(d);

    return TRUE;
}

INT_PTR
k5_handle_wmnc_notify(HWND hwnd,
                      WPARAM wParam, 
                      LPARAM lParam)
{
    switch(HIWORD(wParam)) {
    case WMNC_DIALOG_MOVE:
        {
            k5_dlg_data * d;
                
            d = (k5_dlg_data *)(LONG_PTR) 
                GetWindowLongPtr(hwnd, DWLP_USER);

            if (d->nc->subtype == KMSG_CRED_NEW_CREDS) {
                khui_tracker_reposition(&d->tc_lifetime);
                khui_tracker_reposition(&d->tc_renew);
            }

            return TRUE;
        }
        break;

    case WMNC_DIALOG_SETUP:
        {
            k5_dlg_data * d;

            d = (k5_dlg_data *)(LONG_PTR) 
                GetWindowLongPtr(hwnd, DWLP_USER);

            if (d->nc->subtype == KMSG_CRED_PASSWORD)
                return TRUE;

            /* need to update the controls with d->* */
            if(d->renewable) {
                SendDlgItemMessage(hwnd, IDC_NCK5_RENEWABLE, 
                                   BM_SETCHECK, BST_CHECKED, 
                                   0);
                EnableWindow(GetDlgItem(hwnd, IDC_NCK5_RENEW_EDIT), 
                             TRUE);
            } else {
                SendDlgItemMessage(hwnd, IDC_NCK5_RENEWABLE, 
                                   BM_SETCHECK, BST_UNCHECKED, 0);
                EnableWindow(GetDlgItem(hwnd, IDC_NCK5_RENEW_EDIT), 
                             FALSE);
            }

            khui_tracker_refresh(&d->tc_lifetime);
            khui_tracker_refresh(&d->tc_renew);

            if(d->forwardable) {
                SendDlgItemMessage(hwnd, IDC_NCK5_FORWARDABLE, 
                                   BM_SETCHECK, BST_CHECKED, 0);
            } else {
                SendDlgItemMessage(hwnd, IDC_NCK5_FORWARDABLE, 
                                   BM_SETCHECK, BST_UNCHECKED, 0);
            }
        }
        break;

    case WMNC_UPDATE_CREDTEXT:
        {
            k5_dlg_data * d;
            khui_new_creds * nc;
            khui_new_creds_by_type * nct;
            wchar_t sbuf[1024];
            wchar_t fbuf[256];
            wchar_t tbuf[256];
            size_t cbsize;
            khm_int32 flags;

            d = (k5_dlg_data *)(LONG_PTR) 
                GetWindowLongPtr(hwnd, DWLP_USER);
            nc = d->nc;
            khui_cw_find_type(nc, credtype_id_krb5, &nct);

            if(nct == NULL)
                break;

            if(nct->credtext)
                free(nct->credtext);
            nct->credtext = NULL;

            tbuf[0] = L'\0';

            if (nc->n_identities > 0 &&
                KHM_SUCCEEDED(kcdb_identity_get_flags(nc->identities[0], 
                                                      &flags)) &&
                (flags & KCDB_IDENT_FLAG_VALID) &&
                nc->subtype == KMSG_CRED_NEW_CREDS) {

                if (is_k5_identpro)
                    k5_get_realm_from_nc(nc, tbuf, ARRAYLENGTH(tbuf));
                else
                    GetDlgItemText(hwnd, IDC_NCK5_REALM, tbuf, 
                                   ARRAYLENGTH(tbuf));

                /*TODO: if additional realms were specified, then those
                  must be listed as well */
                LoadString(hResModule, IDS_KRB5_CREDTEXT_0, 
                           fbuf, ARRAYLENGTH(fbuf));
                StringCbPrintf(sbuf, sizeof(sbuf), fbuf, 
                               tbuf);

                StringCbLength(sbuf, sizeof(sbuf), &cbsize);
                cbsize += sizeof(wchar_t);

                nct->credtext = malloc(cbsize);

                StringCbCopy(nct->credtext, cbsize, sbuf);
            } else if (nc->n_identities > 0 &&
                       nc->subtype == KMSG_CRED_PASSWORD) {
                cbsize = sizeof(tbuf);
                kcdb_identity_get_name(nc->identities[0], tbuf, &cbsize);

                LoadString(hResModule, IDS_KRB5_CREDTEXT_P0,
                           fbuf, ARRAYLENGTH(fbuf));
                StringCbPrintf(sbuf, sizeof(sbuf), fbuf, tbuf);

                StringCbLength(sbuf, sizeof(sbuf), &cbsize);
                cbsize += sizeof(wchar_t);

                nct->credtext = malloc(cbsize);

                StringCbCopy(nct->credtext, cbsize, sbuf);
            } else {
                if (d->cred_message) {
                    StringCbLength(d->cred_message, KHUI_MAXCB_BANNER,
                                   &cbsize);
                    cbsize += sizeof(wchar_t);

                    nct->credtext = malloc(cbsize);

                    StringCbCopy(nct->credtext, cbsize, d->cred_message);
                }
            }
        }
        break;

    case WMNC_IDENTITY_CHANGE:
        {
            /* There has been a change of identity */
            k5_dlg_data * d;

            d = (k5_dlg_data *)(LONG_PTR) 
                GetWindowLongPtr(hwnd, DWLP_USER);

            kmq_post_sub_msg(k5_sub, KMSG_CRED, 
                             KMSG_CRED_DIALOG_NEW_IDENTITY, 
                             0, (void *) d->nc);
        }
        break;

    case WMNC_DIALOG_PREPROCESS:
        {
            k5_dlg_data * d;

            d = (k5_dlg_data *)(LONG_PTR) 
                GetWindowLongPtr(hwnd, DWLP_USER);

            if(d->dirty) {
                kmq_post_sub_msg(k5_sub, KMSG_CRED, 
                                 KMSG_CRED_DIALOG_NEW_OPTIONS, 
                                 0, (void *) d->nc);

                /* the above notification effectively takes
                   all our changes into account.  The data we
                   have is no longer dirty */
                d->dirty = FALSE;
            }
        }
        break;
    }

    return 0;
}

INT_PTR
k5_handle_wm_command(HWND hwnd,
                     WPARAM wParam,
                     LPARAM lParam)
{
    int cid;
    int notif;
    k5_dlg_data * d;

    d = (k5_dlg_data *)(LONG_PTR) GetWindowLongPtr(hwnd, DWLP_USER);

    cid = LOWORD(wParam);
    notif = HIWORD(wParam);

    if(notif == BN_CLICKED && cid == IDC_NCK5_RENEWABLE) {
        int c;
        c = (int) SendDlgItemMessage(hwnd, IDC_NCK5_RENEWABLE, 
                                     BM_GETCHECK, 0, 0);
        if(c==BST_CHECKED) {
            EnableWindow(GetDlgItem(hwnd, IDC_NCK5_RENEW_EDIT), TRUE);
            d->renewable = TRUE;
        } else {
            EnableWindow(GetDlgItem(hwnd, IDC_NCK5_RENEW_EDIT), FALSE);
            d->renewable = FALSE;
        }
        d->dirty = TRUE;
    } else if(notif == BN_CLICKED && cid == IDC_NCK5_FORWARDABLE) {
        int c;
        c = (int) SendDlgItemMessage(hwnd, IDC_NCK5_RENEWABLE, 
                                     BM_GETCHECK, 0, 0);
        if(c==BST_CHECKED) {
            d->forwardable = TRUE;
        } else {
            d->forwardable = FALSE;
        }
        d->dirty = TRUE;
    } else if((notif == CBN_SELCHANGE || 
               notif == CBN_KILLFOCUS) && 
              cid == IDC_NCK5_REALM &&
              !is_k5_identpro) {
        /* find out what the realm of the current identity
           is, and if they are the same, then we don't do
           anything */
        wchar_t idname[KCDB_IDENT_MAXCCH_NAME];
        wchar_t realm[KCDB_IDENT_MAXCCH_NAME];
        wchar_t *r;
        khm_size cbsize;
        khm_handle ident;
        int idx;

        if(d->nc->n_identities > 0) {
            if(notif == CBN_SELCHANGE) {
                idx = (int) SendDlgItemMessage(hwnd, IDC_NCK5_REALM, 
                                               CB_GETCURSEL, 0, 0);
                SendDlgItemMessage(hwnd, IDC_NCK5_REALM, 
                                   CB_GETLBTEXT, idx, (LPARAM) realm);
            } else {
                GetDlgItemText(hwnd, IDC_NCK5_REALM, 
                               realm, ARRAYLENGTH(realm));
            }
            cbsize = sizeof(idname);
            if(KHM_SUCCEEDED(kcdb_identity_get_name(d->nc->identities[0], 
                                                  idname, &cbsize))) {
                r = wcschr(idname, L'@');
                if(r && !wcscmp(realm, r+1))
                    return 0; /* nothing to do */

                if(!r) {
                    r = idname + wcslen(idname);
                    *r++ = L'@';
                    *r++ = 0;
                }

                /* if we get here, we have a new user */
                StringCchCopy(r+1, 
                              ARRAYLENGTH(idname) - ((r+1) - idname), 
                              realm);
                if(KHM_SUCCEEDED(kcdb_identity_create(idname, 
                                                    KCDB_IDENT_FLAG_CREATE, 
                                                    &ident))) {
                    khui_cw_set_primary_id(d->nc, ident);
                    kcdb_identity_release(ident);
                }
                return 0;
            }
        }

        /* if we get here, we have a new realm, but there is no
           identity */
        PostMessage(d->nc->hwnd, KHUI_WM_NC_NOTIFY, 
                    MAKEWPARAM(0, WMNC_UPDATE_CREDTEXT), 0);
    }

    return 0;
}
                       

/*  Dialog procedure for the Krb5 credentials type panel.

    NOTE: Runs in the context of the UI thread
*/
INT_PTR CALLBACK 
k5_nc_dlg_proc(HWND hwnd,
               UINT uMsg,
               WPARAM wParam,
               LPARAM lParam)
{
    switch(uMsg) {
    case WM_INITDIALOG: 
        return k5_handle_wm_initdialog(hwnd, wParam, lParam);

    case WM_COMMAND:
        return k5_handle_wm_command(hwnd, wParam, lParam);

    case KHUI_WM_NC_NOTIFY:
        return k5_handle_wmnc_notify(hwnd, wParam, lParam);

    case WM_DESTROY:
        return k5_handle_wm_destroy(hwnd, wParam, lParam);
    }
    return FALSE;
}

/* forward dcl */
krb5_error_code KRB5_CALLCONV 
k5_kinit_prompter(krb5_context context,
                  void *data,
                  const char *name,
                  const char *banner,
                  int num_prompts,
                  krb5_prompt prompts[]);



fiber_job g_fjob;   /* global fiber job object */

static BOOL 
k5_cached_kinit_prompter(void);

static BOOL
k5_cp_check_continue(void);

/*
    Runs in the context of the krb5 plugin's slave fiber
*/
VOID CALLBACK 
k5_kinit_fiber_proc(PVOID lpParameter)
{
    while(TRUE)
    {
        if(g_fjob.command == FIBER_CMD_KINIT) {
            g_fjob.state = FIBER_STATE_KINIT;

            g_fjob.prompt_set = 0;

            if (k5_cached_kinit_prompter()) {
                SwitchToFiber(k5_main_fiber);

                if (g_fjob.command != FIBER_CMD_CONTINUE)
                    goto _switch_to_main;

                if (!k5_cp_check_continue()) {
                    goto _switch_to_main;
                }
            }

            g_fjob.code = khm_krb5_kinit(
                0,
                g_fjob.principal,
                g_fjob.password,
                g_fjob.ccache,
                g_fjob.lifetime,
                g_fjob.forwardable,
                g_fjob.proxiable,
                (g_fjob.renewable ? g_fjob.renew_life : 0),
                g_fjob.addressless,
                g_fjob.publicIP,
                k5_kinit_prompter,
                &g_fjob);
        }

    _switch_to_main:
        g_fjob.state = FIBER_STATE_NONE;

        SwitchToFiber(k5_main_fiber);
    }
}

/* return TRUE if we should go ahead with creds acquisition */
static BOOL
k5_cp_check_continue(void) {
    khm_size i;
    khm_size n_p;
    khui_new_creds_prompt * p;
    size_t cch;

#ifdef DEBUG
    assert(g_fjob.nc);
#endif

    if (KHM_FAILED(khui_cw_get_prompt_count(g_fjob.nc, &n_p))) {
#ifdef DEBUG
        assert(FALSE);
#endif
        return TRUE;
    }

    khui_cw_sync_prompt_values(g_fjob.nc);

    g_fjob.null_password = FALSE;

    /* we are just checking whether there was a password field that
       was left empty, in which case we can't continue with the
       credentials acquisition. */
    for (i=0; i < n_p; i++) {
        if(KHM_FAILED(khui_cw_get_prompt(g_fjob.nc,
                                         (int) i,
                                         &p)))
            continue;
        if(p->type == KHUI_NCPROMPT_TYPE_PASSWORD) {
            if (p->value == NULL ||
                FAILED(StringCchLength(p->value, KHUI_MAXCCH_PROMPT,
                                       &cch)) ||
                cch == 0) {
                g_fjob.null_password = TRUE;
                return FALSE;
            } else
                break;
        }
    }

    return TRUE;
}

/* returns true if we find cached prompts */
static BOOL 
k5_cached_kinit_prompter(void) {
    BOOL rv = FALSE;
    khm_handle ident;
    khm_handle csp_idconfig = NULL;
    khm_handle csp_k5config = NULL;
    khm_handle csp_prcache = NULL;
    khm_size cb;
    khm_size n_cur_prompts;
    khm_int32 n_prompts;
    khm_int32 i;

#ifdef DEBUG
    assert(g_fjob.nc);
#endif

    ident = g_fjob.identity;
    if (!ident)
        return FALSE;

    /* don't need to hold ident, since it is already held in g_fjob
       and it doesn't change until we return */

    if (KHM_FAILED(kcdb_identity_get_config(ident, 0, &csp_idconfig)) ||

        KHM_FAILED(khc_open_space(csp_idconfig, CSNAME_KRB5CRED,
                                  0, &csp_k5config)) ||

        KHM_FAILED(khc_open_space(csp_k5config, CSNAME_PROMPTCACHE,
                                  0, &csp_prcache)) ||

        KHM_FAILED(khc_read_int32(csp_prcache, L"PromptCount",
                                  &n_prompts)) ||
        n_prompts == 0)

        goto _cleanup;

    /* check if there are any prompts currently showing.  If there are
       we check if they are the same as the ones we are going to show.
       In which case we just reuse the exisitng prompts */
    if (KHM_FAILED(khui_cw_get_prompt_count(g_fjob.nc, 
                                            &n_cur_prompts)) ||
        n_prompts != (khm_int32) n_cur_prompts)
        goto _show_new_prompts;

    for(i = 0; i < n_prompts; i++) {
        wchar_t wsname[8];
        wchar_t wprompt[KHUI_MAXCCH_PROMPT];
        khm_handle csp_p = NULL;
        khm_int32 p_type;
        khm_int32 p_flags;
        khui_new_creds_prompt * p;

        if (KHM_FAILED(khui_cw_get_prompt(g_fjob.nc, i, &p)))
            break;

        StringCbPrintf(wsname, sizeof(wsname), L"%d", i);

        if (KHM_FAILED(khc_open_space(csp_prcache, wsname, 0, &csp_p)))
            break;

        cb = sizeof(wprompt);
        if (KHM_FAILED(khc_read_string(csp_p, L"Prompt", 
                                       wprompt, &cb))) {
            khc_close_space(csp_p);
            break;
        }

        if (KHM_FAILED(khc_read_int32(csp_p, L"Type", &p_type)))
            p_type = 0;

        if (KHM_FAILED(khc_read_int32(csp_p, L"Flags", &p_flags)))
            p_flags = 0;

        if (                    /* if we received a prompt string,
                                   then it should be the same as the
                                   one that is displayed */
            (wprompt[0] &&
             (p->prompt == NULL ||
              wcscmp(wprompt, p->prompt))) ||

                                /* if we didn't receive one, then
                                   there shouldn't be one displayed.
                                   This case really shouldn't happen
                                   in reality, but we check anyway. */
            (!wprompt[0] &&
             p->prompt != NULL) ||

                                /* the type should match */
            (p_type != p->type) ||

                                /* if this prompt should be hidden,
                                   then it must also be so */
            (p_flags != p->flags)
            ) {

            khc_close_space(csp_p);
            break;

        }
        

        khc_close_space(csp_p);
    }

    if (i == n_prompts) {
        rv = TRUE;
        goto _cleanup;
    }

 _show_new_prompts:

    khui_cw_clear_prompts(g_fjob.nc);

    {
        wchar_t wbanner[KHUI_MAXCCH_BANNER];
        wchar_t wpname[KHUI_MAXCCH_PNAME];

        cb = sizeof(wbanner);
        if (KHM_FAILED(khc_read_string(csp_prcache, L"Banner", 
                                      wbanner, &cb)))
            wbanner[0] = 0;

        cb = sizeof(wpname);
        if (KHM_FAILED(khc_read_string(csp_prcache, L"Name",
                                       wpname, &cb)))
            wpname[0] = 0;

        khui_cw_begin_custom_prompts(g_fjob.nc,
                                     n_prompts,
                                     (wbanner[0]? wbanner: NULL),
                                     (wpname[0]? wpname: NULL));
    }

    for(i = 0; i < n_prompts; i++) {
        wchar_t wsname[8];
        wchar_t wprompt[KHUI_MAXCCH_PROMPT];
        khm_handle csp_p = NULL;
        khm_int32 p_type;
        khm_int32 p_flags;

        StringCbPrintf(wsname, sizeof(wsname), L"%d", i);

        if (KHM_FAILED(khc_open_space(csp_prcache, wsname, 0, &csp_p)))
            break;

        cb = sizeof(wprompt);
        if (KHM_FAILED(khc_read_string(csp_p, L"Prompt", 
                                       wprompt, &cb))) {
            khc_close_space(csp_p);
            break;
        }

        if (KHM_FAILED(khc_read_int32(csp_p, L"Type", &p_type)))
            p_type = 0;

        if (KHM_FAILED(khc_read_int32(csp_p, L"Flags", &p_flags)))
            p_flags = 0;

        khui_cw_add_prompt(g_fjob.nc, p_type, wprompt, NULL, p_flags);

        khc_close_space(csp_p);
    }

    if (i < n_prompts) {
        khui_cw_clear_prompts(g_fjob.nc);
    } else {
        rv = TRUE;
    }
     
 _cleanup:

    if (csp_prcache)
        khc_close_space(csp_prcache);

    if (csp_k5config)
        khc_close_space(csp_k5config);

    if (csp_idconfig)
        khc_close_space(csp_idconfig);

    return rv;
}

/*  Runs in the context of the Krb5 plugin's slave fiber */
krb5_error_code KRB5_CALLCONV 
k5_kinit_prompter(krb5_context context,
                  void *data,
                  const char *name,
                  const char *banner,
                  int num_prompts,
                  krb5_prompt prompts[])
{
    int i;
    khui_new_creds * nc;
    krb5_prompt_type * ptypes;
    khm_size ncp;
    krb5_error_code code = 0;
    BOOL new_prompts = TRUE;

    khm_handle csp_prcache = NULL;

    nc = g_fjob.nc;

    if(pkrb5_get_prompt_types)
        ptypes = pkrb5_get_prompt_types(context);
    else
        ptypes = NULL;

    /* check if we are already showing the right prompts */
    khui_cw_get_prompt_count(nc, &ncp);

    if (num_prompts != (int) ncp)
        goto _show_new_prompts;

    for (i=0; i < num_prompts; i++) {
        wchar_t wprompt[KHUI_MAXCCH_PROMPT];
        khui_new_creds_prompt * p;

        if(prompts[i].prompt) {
            AnsiStrToUnicode(wprompt, sizeof(wprompt), 
                             prompts[i].prompt);
        } else {
            wprompt[0] = 0;
        }

        if (KHM_FAILED(khui_cw_get_prompt(nc, i, &p)))
            break;

        if (                    /* if we received a prompt string,
                                   then it should be the same as the
                                   one that is displayed */
            (wprompt[0] &&
             (p->prompt == NULL ||
              wcscmp(wprompt, p->prompt))) ||
                                /* if we didn't receive one, then
                                   there shouldn't be one displayed.
                                   This case really shouldn't happen
                                   in reality, but we check anyway. */
            (!wprompt[0] &&
             p->prompt != NULL) ||
                                /* the type should match */
            (ptypes &&
             ptypes[i] != p->type) ||
            (!ptypes &&
             p->type != 0) ||
                                /* if this prompt should be hidden,
                                   then it must also be so */
            (prompts[i].hidden &&
             !(p->flags & KHUI_NCPROMPT_FLAG_HIDDEN)) ||
            (!prompts[i].hidden &&
             (p->flags & KHUI_NCPROMPT_FLAG_HIDDEN))
            )
            break;
    }

    if (i < num_prompts)
        goto _show_new_prompts;

    new_prompts = FALSE;

    /* ok. looks like we are already showing the same set of prompts
       that we were supposed to show.  Sync up the values and go
       ahead. */
    khui_cw_sync_prompt_values(nc);
    goto _process_prompts;

 _show_new_prompts:
    /* special case.  if there are no actual input controls involved,
       then we have to show an alerter window and pass through */
    if (num_prompts == 0) {
        wchar_t wbanner[KHUI_MAXCCH_BANNER];
        wchar_t wname[KHUI_MAXCCH_PNAME];
        wchar_t wident[KCDB_IDENT_MAXCCH_NAME];
        wchar_t wmsg[KHUI_MAXCCH_MESSAGE];
        wchar_t wfmt[KHUI_MAXCCH_BANNER];
        khm_size cb;

        if (!banner) {
            code = 0;
            g_fjob.null_password = FALSE;
            goto _exit;
        } else {
            AnsiStrToUnicode(wbanner, sizeof(wbanner), banner);
        }

        if (name) {
            AnsiStrToUnicode(wname, sizeof(wname), name);
        } else {
            LoadString(hResModule,
                       IDS_KRB5_WARNING,
                       wname,
                       ARRAYLENGTH(wname));
        }

        cb = sizeof(wident);
        if (KHM_FAILED(kcdb_identity_get_name(g_fjob.identity, wident, &cb)))
            wident[0] = L'\0';

        LoadString(hResModule,
                   IDS_KRB5_WARN_FMT,
                   wfmt,
                   ARRAYLENGTH(wfmt));

        StringCbPrintf(wmsg, sizeof(wmsg), wfmt, wident, wbanner);

        khui_alert_show_simple(wname, wmsg, KHERR_WARNING);

        code = 0;
        g_fjob.null_password = FALSE;
        goto _exit;
    }

    /* in addition to showing new prompts, we also cache the set of
       prompts. */
    if(g_fjob.prompt_set == 0) {
        khm_handle csp_idconfig = NULL;
        khm_handle csp_idk5 = NULL;

        kcdb_identity_get_config(g_fjob.identity,
                                 KHM_FLAG_CREATE,
                                 &csp_idconfig);

        if (csp_idconfig != NULL)
            khc_open_space(csp_idconfig,
                           CSNAME_KRB5CRED,
                           KHM_FLAG_CREATE,
                           &csp_idk5);

        if (csp_idk5 != NULL)
            khc_open_space(csp_idk5,
                           CSNAME_PROMPTCACHE,
                           KHM_FLAG_CREATE,
                           &csp_prcache);

        khc_close_space(csp_idconfig);
        khc_close_space(csp_idk5);
    }

    {
        wchar_t wbanner[KHUI_MAXCCH_BANNER];
        wchar_t wname[KHUI_MAXCCH_PNAME];

        if(banner)
            AnsiStrToUnicode(wbanner, sizeof(wbanner), banner);
        if(name)
            AnsiStrToUnicode(wname, sizeof(wname), name);

        khui_cw_clear_prompts(nc);

        khui_cw_begin_custom_prompts(
            nc, 
            num_prompts, 
            (banner)?wbanner:NULL,
            (name)?wname:NULL);

        if (banner && csp_prcache)
            khc_write_string(csp_prcache,
                             L"Banner",
                             wbanner);
        else if (csp_prcache)
            khc_write_string(csp_prcache,
                             L"Banner",
                             L"");

        if (name && csp_prcache)
            khc_write_string(csp_prcache,
                             L"Name",
                             wname);
        else if (csp_prcache)
            khc_write_string(csp_prcache,
                             L"Name",
                             L"");

        if (csp_prcache)
            khc_write_int32(csp_prcache,
                            L"PromptCount",
                            (khm_int32) num_prompts);
    }

    for(i=0; i < num_prompts; i++) {
        wchar_t wprompt[KHUI_MAXCCH_PROMPT];

        if(prompts[i].prompt) {
            AnsiStrToUnicode(wprompt, sizeof(wprompt), 
                             prompts[i].prompt);
        } else {
            wprompt[0] = 0;
        }

        khui_cw_add_prompt(
            nc,
            (ptypes?ptypes[i]:0),
            wprompt,
            NULL,
            (prompts[i].hidden?KHUI_NCPROMPT_FLAG_HIDDEN:0));

        if (csp_prcache) {
            khm_handle csp_p = NULL;
            wchar_t wnum[8];    /* should be enough for 10
                                   million prompts */

            wnum[0] = 0;
            StringCbPrintf(wnum, sizeof(wnum), L"%d", i);

            khc_open_space(csp_prcache, wnum, 
                           KHM_FLAG_CREATE, &csp_p);

            if (csp_p) {
                khc_write_string(csp_p, L"Prompt", wprompt);
                khc_write_int32(csp_p, L"Type", (ptypes?ptypes[i]:0));
                khc_write_int32(csp_p, L"Flags",
                                (prompts[i].hidden?
                                 KHUI_NCPROMPT_FLAG_HIDDEN:0));

                khc_close_space(csp_p);
            }
        }
    }

    if (csp_prcache) {
        khc_close_space(csp_prcache);
        csp_prcache = NULL;
    }

 _process_prompts:
    /* switch back to main thread if we showed new prompts */
    if (new_prompts)
        SwitchToFiber(k5_main_fiber);

    /* we get here after the user selects an action that either
       cancles the credentials acquisition operation or triggers the
       actual acquisition of credentials. */
    if(g_fjob.command != FIBER_CMD_CONTINUE &&
       g_fjob.command != FIBER_CMD_KINIT) {
        code = -2;
        goto _exit;
    }

    g_fjob.null_password = FALSE;

    /* otherwise, we need to get the data back from the UI and
       return 0 */
    for(i=0; i<num_prompts; i++) {
        krb5_data * d;
        wchar_t wbuf[512];
        khm_size cbbuf;
        size_t cch;

        d = prompts[i].reply;

        cbbuf = sizeof(wbuf);
        if(KHM_SUCCEEDED(khui_cw_get_prompt_value(nc, i, wbuf, &cbbuf))) {
            UnicodeStrToAnsi(d->data, d->length, wbuf);
            if(SUCCEEDED(StringCchLengthA(d->data, d->length, &cch)))
                d->length = (unsigned int) cch;
            else
                d->length = 0;
        } else {
#ifdef DEBUG
            assert(FALSE);
#endif
            d->length = 0;
        }

        if (ptypes && 
            ptypes[i] == KRB5_PROMPT_TYPE_PASSWORD &&
            d->length == 0)

            g_fjob.null_password = TRUE;
    }

 _exit:

    g_fjob.prompt_set++;

    /* entering a NULL password is equivalent to cancelling out */
    if (g_fjob.null_password)
        return -2;
    else
        return code;
}


void 
k5_read_dlg_params(khm_handle conf, 
                   k5_dlg_data * d)
{
    khm_int32 i;

    khc_read_int32(conf, L"Renewable", &d->renewable);
    khc_read_int32(conf, L"Forwardable", &d->forwardable);
    khc_read_int32(conf, L"Proxiable", &d->proxiable);
    khc_read_int32(conf, L"Addressless", &d->addressless);

    khc_read_int32(conf, L"DefaultLifetime", &i);
    d->tc_lifetime.current = i;
    khc_read_int32(conf, L"MaxLifetime", &i);
    d->tc_lifetime.max = i;
    khc_read_int32(conf, L"MinLifetime", &i);
    d->tc_lifetime.min = i;

    khc_read_int32(conf, L"DefaultRenewLifetime", &i);
    d->tc_renew.current = i;
    khc_read_int32(conf, L"MaxRenewLifetime", &i);
    d->tc_renew.max = i;
    khc_read_int32(conf, L"MinRenewLifetime", &i);
    d->tc_renew.min = i;

    /* however, if this has externally supplied defaults, we have to
       use them too. */
    if (d->nc && d->nc->ctx.vparam &&
        d->nc->ctx.cb_vparam == sizeof(NETID_DLGINFO)) {
        LPNETID_DLGINFO pdlginfo;

        pdlginfo = (LPNETID_DLGINFO) d->nc->ctx.vparam;
        if (pdlginfo->size == NETID_DLGINFO_V1_SZ &&
            pdlginfo->in.use_defaults == 0) {
            d->forwardable = pdlginfo->in.forwardable;
            d->addressless = pdlginfo->in.noaddresses;
            d->tc_lifetime.current = pdlginfo->in.lifetime;
            d->tc_renew.current = pdlginfo->in.renew_till;

            if (pdlginfo->in.renew_till == 0)
                d->renewable = FALSE;
            else
                d->renewable = TRUE;

            d->proxiable = pdlginfo->in.proxiable;
            d->publicIP = pdlginfo->in.publicip;
        }
    }

    /* once we read the new data, in, it is no longer considered
       dirty */
    d->dirty = FALSE;
}

void 
k5_write_dlg_params(khm_handle conf, 
                    k5_dlg_data * d)
{
    khc_write_int32(conf, L"Renewable", d->renewable);
    khc_write_int32(conf, L"Forwardable", d->forwardable);
    khc_write_int32(conf, L"Proxiable", d->proxiable);
    khc_write_int32(conf, L"Addressless", d->addressless);

    khc_write_int32(conf, L"DefaultLifetime", 
                    (khm_int32) d->tc_lifetime.current);
    khc_write_int32(conf, L"MaxLifetime", 
                    (khm_int32) d->tc_lifetime.max);
    khc_write_int32(conf, L"MinLifetime", 
                    (khm_int32) d->tc_lifetime.min);

    khc_write_int32(conf, L"DefaultRenewLifetime", 
                    (khm_int32) d->tc_renew.current);
    khc_write_int32(conf, L"MaxRenewLifetime", 
                    (khm_int32) d->tc_renew.max);
    khc_write_int32(conf, L"MinRenewLifetime", 
                    (khm_int32) d->tc_renew.min);

    /* as in k5_read_dlg_params, once we write the data in, the local
       data is no longer dirty */
    d->dirty = FALSE;
}

void 
k5_prep_kinit_job(khui_new_creds * nc)
{
    khui_new_creds_by_type * nct;
    k5_dlg_data * d;
    wchar_t idname[KCDB_IDENT_MAXCCH_NAME];
    khm_size cbbuf;
    size_t size;
    khm_handle ident;
    LPNETID_DLGINFO pdlginfo;

    khui_cw_find_type(nc, credtype_id_krb5, &nct);
    if (!nct)
        return;

    d = (k5_dlg_data *)(LONG_PTR) 
        GetWindowLongPtr(nct->hwnd_panel, DWLP_USER);

    khui_cw_lock_nc(nc);
    ident = nc->identities[0];
    kcdb_identity_hold(ident);
    khui_cw_unlock_nc(nc);

    cbbuf = sizeof(idname);
    kcdb_identity_get_name(ident, idname, &cbbuf);
    StringCchLength(idname, ARRAYLENGTH(idname), &size);
    size++;
    
    ZeroMemory(&g_fjob, sizeof(g_fjob));
    g_fjob.command = FIBER_CMD_KINIT;
    g_fjob.nc = nc;
    g_fjob.nct = nct;
    g_fjob.dialog = nct->hwnd_panel;
    g_fjob.principal = malloc(size);
    UnicodeStrToAnsi(g_fjob.principal, size, idname);
    g_fjob.password = NULL;
    g_fjob.lifetime = (krb5_deltat) d->tc_lifetime.current;
    g_fjob.forwardable = d->forwardable;
    g_fjob.proxiable = d->proxiable;
    g_fjob.renewable = d->renewable;
    g_fjob.renew_life = (krb5_deltat) d->tc_renew.current;
    g_fjob.addressless = d->addressless;
    g_fjob.publicIP = 0;
    g_fjob.code = 0;
    g_fjob.identity = ident;
    g_fjob.prompt_set = 0;

    /* if we have external parameters, we should use them as well */
    if (nc->ctx.cb_vparam == sizeof(NETID_DLGINFO) &&
        (pdlginfo = nc->ctx.vparam) &&
        pdlginfo->size == NETID_DLGINFO_V1_SZ) {
        wchar_t * t;

        if (pdlginfo->in.ccache[0] &&
            SUCCEEDED(StringCchLength(pdlginfo->in.ccache,
                                      NETID_CCACHE_NAME_SZ,
                                      &size))) {
            g_fjob.ccache = malloc(sizeof(char) * (size + 1));
#ifdef DEBUG
            assert(g_fjob.ccache);
#endif
            UnicodeStrToAnsi(g_fjob.ccache, size + 1,
                             pdlginfo->in.ccache);

            /* this is the same as the output cache */

            StringCbCopy(pdlginfo->out.ccache, sizeof(pdlginfo->out.ccache),
                         pdlginfo->in.ccache);
        } else {
            g_fjob.ccache = NULL;

            StringCbCopy(pdlginfo->out.ccache, sizeof(pdlginfo->out.ccache),
                         idname);

            khm_krb5_canon_cc_name(pdlginfo->out.ccache,
                                   sizeof(pdlginfo->out.ccache));
        }

        t = khm_get_realm_from_princ(idname);

        if (t) {
            StringCbCopy(pdlginfo->out.realm,
                         sizeof(pdlginfo->out.realm),
                         t);

            if ((t - idname) > 1) {
                StringCchCopyN(pdlginfo->out.username,
                               ARRAYLENGTH(pdlginfo->out.username),
                               idname,
                               (t - idname) - 1);
            } else {
                StringCbCopy(pdlginfo->out.username,
                             sizeof(pdlginfo->out.username),
                             L"");
            }
        } else {
            StringCbCopy(pdlginfo->out.username,
                         sizeof(pdlginfo->out.username),
                         idname);
            StringCbCopy(pdlginfo->out.realm,
                         sizeof(pdlginfo->out.realm),
                         L"");
        }
    }

    /* leave identity held, since we added a reference above */
}

void 
k5_free_kinit_job(void)
{
    if (g_fjob.principal)
        free(g_fjob.principal);

    if (g_fjob.password)
        free(g_fjob.password);

    if (g_fjob.identity)
        kcdb_identity_release(g_fjob.identity);

    if (g_fjob.ccache)
        free(g_fjob.ccache);

    ZeroMemory(&g_fjob, sizeof(g_fjob));
}

static khm_int32 KHMAPI 
k5_find_tgt_filter(khm_handle cred,
                   khm_int32 flags,
                   void * rock) {
    khm_handle ident = (khm_handle) rock;
    khm_handle cident = NULL;
    khm_int32 f;
    khm_int32 rv;

    if (KHM_SUCCEEDED(kcdb_cred_get_identity(cred,
                                           &cident)) &&
        cident == ident &&
        KHM_SUCCEEDED(kcdb_cred_get_flags(cred, &f)) &&
        (f & KCDB_CRED_FLAG_INITIAL))
        rv = 1;
    else
        rv = 0;

    if (cident)
        kcdb_identity_release(cident);

    return rv;
}

/* Handler for CRED type messages

    Runs in the context of the Krb5 plugin
*/
khm_int32 KHMAPI 
k5_msg_cred_dialog(khm_int32 msg_type, 
                   khm_int32 msg_subtype, 
                   khm_ui_4 uparam, 
                   void * vparam)
{
    khm_int32 rv = KHM_ERROR_SUCCESS;

    switch(msg_subtype) {

    case KMSG_CRED_PASSWORD:
    case KMSG_CRED_NEW_CREDS:
        {
            khui_new_creds * nc;
            khui_new_creds_by_type * nct;
            wchar_t wbuf[256];
            size_t cbsize;

            nc = (khui_new_creds *) vparam;

            nct = malloc(sizeof(*nct));
            ZeroMemory(nct, sizeof(*nct));

            nct->type = credtype_id_krb5;
            nct->ordinal = 1;

            LoadString(hResModule, IDS_KRB5_SHORT_DESC, 
                       wbuf, ARRAYLENGTH(wbuf));
            StringCbLength(wbuf, sizeof(wbuf), &cbsize);
            cbsize += sizeof(wchar_t);

            nct->name = malloc(cbsize);
            StringCbCopy(nct->name, cbsize, wbuf);

            nct->h_module = hResModule;
            nct->dlg_proc = k5_nc_dlg_proc;
            if (nc->subtype == KMSG_CRED_PASSWORD)
                nct->dlg_template = MAKEINTRESOURCE(IDD_NC_KRB5_PASSWORD);
            else
                nct->dlg_template = MAKEINTRESOURCE(IDD_NC_KRB5);

            khui_cw_add_type(nc, nct);
        }
        break;

    case KMSG_CRED_RENEW_CREDS:
        {
            khui_new_creds * nc;
            khui_new_creds_by_type * nct;

            nc = (khui_new_creds *) vparam;

            nct = malloc(sizeof(*nct));
            ZeroMemory(nct, sizeof(*nct));

            nct->type = credtype_id_krb5;

            khui_cw_add_type(nc, nct);
        }
        break;

    case KMSG_CRED_DIALOG_PRESTART:
        {
            khui_new_creds * nc;
            khui_new_creds_by_type * nct;
            k5_dlg_data * d;
            HWND hwnd;
            wchar_t * realms;
            wchar_t * t;
            wchar_t * defrealm;

            nc = (khui_new_creds *) vparam;

            khui_cw_find_type(nc, credtype_id_krb5, &nct);

            if(!nct)
                break;

            hwnd = nct->hwnd_panel;
            d = (k5_dlg_data *)(LONG_PTR) 
                GetWindowLongPtr(nct->hwnd_panel, DWLP_USER);

            if (!is_k5_identpro) {

                /* enumerate all realms and place in realms combo box */
                SendDlgItemMessage(hwnd, IDC_NCK5_REALM, 
                                   CB_RESETCONTENT, 
                                   0, 0);

                realms = khm_krb5_get_realm_list();
                if(realms) {
                    t = realms;
                    while(t && *t) {
                        SendDlgItemMessage(hwnd, IDC_NCK5_REALM, 
                                           CB_ADDSTRING,
                                           0, (LPARAM) t);
                        t = multi_string_next(t);
                    }
                    free(realms);
                }

                /* and set the default realm */
                defrealm = khm_krb5_get_default_realm();
                if(defrealm) {
                    SendDlgItemMessage(hwnd, IDC_NCK5_REALM,
                                       CB_SELECTSTRING,
                                       (WPARAM) -1,
                                       (LPARAM) defrealm);

                    SendDlgItemMessage(hwnd, IDC_NCK5_REALM, 
                                       WM_SETTEXT, 
                                       0, (LPARAM) defrealm);
                    free(defrealm);
                }
            } else {            /* if krb5 is the identity provider */
                HWND hw_realms;

                /* in this case, the realm selection is done by the
                   identity provider prompts. */

                hw_realms = GetDlgItem(hwnd, IDC_NCK5_REALM);
#ifdef DEBUG
                assert(hw_realms);
#endif
                EnableWindow(hw_realms, FALSE);
            }

            if (nc->subtype == KMSG_CRED_NEW_CREDS) {
                k5_read_dlg_params(csp_params, d);
            }

            PostMessage(hwnd, KHUI_WM_NC_NOTIFY, 
                        MAKEWPARAM(0,WMNC_DIALOG_SETUP), 0);
        }
        break;
            
    case KMSG_CRED_DIALOG_NEW_IDENTITY:
        {
            khui_new_creds * nc;
            khui_new_creds_by_type * nct;
            k5_dlg_data * d;
            
            nc = (khui_new_creds *) vparam;

            khui_cw_find_type(nc, credtype_id_krb5, &nct);
            if (!nct)
                break;

            d = (k5_dlg_data *)(LONG_PTR) 
                GetWindowLongPtr(nct->hwnd_panel, DWLP_USER);

            /* we only load the identity specific defaults if the user
               hasn't changed the options */
            khui_cw_lock_nc(nc);

            if(!d->dirty && nc->n_identities > 0 &&
               nc->subtype == KMSG_CRED_NEW_CREDS) {

                khm_handle h_id = NULL;
                khm_handle h_idk5 = NULL;

                do {
                    if(KHM_FAILED
                       (kcdb_identity_get_config(nc->identities[0],
                                                 0,
                                                 &h_id)))
                        break;

                    if(KHM_FAILED
                       (khc_open_space(h_id, CSNAME_KRB5CRED, 
                                       0, &h_idk5)))
                        break;

                    if(KHM_FAILED(khc_shadow_space(h_idk5, csp_params)))
                        break;

                    k5_read_dlg_params(h_idk5, d);

                    PostMessage(nct->hwnd_panel, KHUI_WM_NC_NOTIFY, 
                                MAKEWPARAM(0,WMNC_DIALOG_SETUP), 0);
                } while(FALSE);

                if(h_id)
                    khc_close_space(h_id);
                if(h_idk5)
                    khc_close_space(h_idk5);
            }

            khui_cw_unlock_nc(nc);
        }

        /* fallthrough */
    case KMSG_CRED_DIALOG_NEW_OPTIONS:
        {
            khui_new_creds * nc;
            khui_new_creds_by_type * nct;
            k5_dlg_data * d;

            nc = (khui_new_creds *) vparam;

            khui_cw_find_type(nc, credtype_id_krb5, &nct);
            if (!nct)
                break;

            d = (k5_dlg_data *)(LONG_PTR) 
                GetWindowLongPtr(nct->hwnd_panel, DWLP_USER);

            if (nc->subtype == KMSG_CRED_PASSWORD) {
                khm_size n_prompts = 0;

                khui_cw_get_prompt_count(nc, &n_prompts);

                if (nc->n_identities == 0) {
                    if (n_prompts)
                        khui_cw_clear_prompts(nc);
                } else if (n_prompts != 3) {
                    wchar_t wbuf[KHUI_MAXCCH_BANNER];

                    khui_cw_clear_prompts(nc);

                    LoadString(hResModule, IDS_NC_PWD_BANNER,
                               wbuf, ARRAYLENGTH(wbuf));
                    khui_cw_begin_custom_prompts(nc, 3, NULL, wbuf);

                    LoadString(hResModule, IDS_NC_PWD_PWD,
                               wbuf, ARRAYLENGTH(wbuf));
                    khui_cw_add_prompt(nc, KHUI_NCPROMPT_TYPE_PASSWORD, 
                                       wbuf, NULL, KHUI_NCPROMPT_FLAG_HIDDEN);

                    LoadString(hResModule, IDS_NC_PWD_NPWD,
                               wbuf, ARRAYLENGTH(wbuf));
                    khui_cw_add_prompt(nc, KHUI_NCPROMPT_TYPE_NEW_PASSWORD,
                                       wbuf, NULL, KHUI_NCPROMPT_FLAG_HIDDEN);

                    LoadString(hResModule, IDS_NC_PWD_NPWD_AGAIN,
                               wbuf, ARRAYLENGTH(wbuf));
                    khui_cw_add_prompt(nc, KHUI_NCPROMPT_TYPE_NEW_PASSWORD_AGAIN,
                                       wbuf, NULL, KHUI_NCPROMPT_FLAG_HIDDEN);
                }

                return KHM_ERROR_SUCCESS;
            }
            /* else; nc->subtype == KMSG_CRED_NEW_CREDS */

            assert(nc->subtype == KMSG_CRED_NEW_CREDS);

            /* if the fiber is already in a kinit, cancel it */
            if(g_fjob.state == FIBER_STATE_KINIT) {
                g_fjob.command = FIBER_CMD_CANCEL;
                SwitchToFiber(k5_kinit_fiber);
                /* we get here when the cancel operation completes */
                k5_free_kinit_job();
            }

            khui_cw_lock_nc(nc);

            if(nc->n_identities > 0) {
                khm_handle ident = nc->identities[0];

                kcdb_identity_hold(ident);

                k5_prep_kinit_job(nc);
                khui_cw_unlock_nc(nc);

                SwitchToFiber(k5_kinit_fiber);
                /* we get here when the fiber switches back */
                if(g_fjob.state == FIBER_STATE_NONE) {
                    wchar_t msg[KHUI_MAXCCH_BANNER];
                    khm_size cb;

                    /* we can't possibly have succeeded without a
                       password */
                    if(g_fjob.code) {
                        if (is_k5_identpro)
                            kcdb_identity_set_flags(ident, 
                                                    KCDB_IDENT_FLAG_INVALID);

                        khui_cw_clear_prompts(nc);
                    }

                    if (d->cred_message) {
                        free(d->cred_message);
                        d->cred_message = NULL;
                    }

                    msg[0] = L'\0';

                    switch(g_fjob.code) {
                    case KRB5KDC_ERR_NAME_EXP:
                        /* principal expired */
                        LoadString(hResModule, IDS_K5ERR_NAME_EXPIRED,
                                   msg, ARRAYLENGTH(msg));
                        break;

                    case KRB5KDC_ERR_KEY_EXP:
                        /* password needs changing */
                        LoadString(hResModule, IDS_K5ERR_KEY_EXPIRED,
                                   msg, ARRAYLENGTH(msg));
                        break;

                    default:
                        {
                            DWORD dw_dummy;
                            kherr_suggestion sug_dummy;
                            wchar_t fmt[KHUI_MAXCCH_BANNER];
                            wchar_t desc[KHUI_MAXCCH_BANNER];

                            LoadString(hResModule, IDS_K5ERR_FMT,
                                       fmt, ARRAYLENGTH(fmt));

                            khm_err_describe(g_fjob.code,
                                             desc,
                                             sizeof(desc),
                                             &dw_dummy,
                                             &sug_dummy);

                            StringCbPrintf(msg, sizeof(msg), fmt, desc);
                        }
                    }

                    if (msg[0]) {
                        StringCbLength(msg, sizeof(msg), &cb);
                        cb += sizeof(wchar_t);

                        d->cred_message = malloc(cb);
                        StringCbCopy(d->cred_message, cb, msg);
                    }

                    k5_free_kinit_job();

                } else if(g_fjob.state == FIBER_STATE_KINIT) {
                    /* this is what we want.  Leave the fiber there. */

                    if(is_k5_identpro)
                        kcdb_identity_set_flags(ident, 
                                                KCDB_IDENT_FLAG_VALID);
                } else {
                    /* huh?? */
#ifdef DEBUG
                    assert(FALSE);
#endif
                }

                /* since the attributes of the identity have changed,
                   we should update the cred text as well */
                kcdb_identity_release(ident);
                khui_cw_lock_nc(nc);
                PostMessage(nc->hwnd, KHUI_WM_NC_NOTIFY, 
                            MAKEWPARAM(0, WMNC_UPDATE_CREDTEXT), 0);
            } else {
                khui_cw_unlock_nc(nc);
                khui_cw_clear_prompts(nc);
                khui_cw_lock_nc(nc);
            }

            khui_cw_unlock_nc(nc);
        }
        break;

    case KMSG_CRED_PROCESS:
        {
            khui_new_creds * nc;
            khui_new_creds_by_type * nct;
            k5_dlg_data * d;

            khm_int32 r;

            nc = (khui_new_creds *) vparam;

            khui_cw_find_type(nc, credtype_id_krb5, &nct);

            if(!nct)
                break;

            /* reset the null_password flag, just in case */
            g_fjob.null_password = FALSE;

            if (nc->subtype == KMSG_CRED_NEW_CREDS) {
                d = (k5_dlg_data *) nct->aux;

                _begin_task(0);
                _report_mr0(KHERR_NONE, MSG_CTX_INITAL_CREDS);
                _describe();

                if (g_fjob.state == FIBER_STATE_KINIT) {
                    if(nc->result == KHUI_NC_RESULT_CANCEL) {
                        g_fjob.command = FIBER_CMD_CANCEL;
                        SwitchToFiber(k5_kinit_fiber);

                        /* if we cancelled out, then we shouldn't care
                           about the return code. */
#ifdef DEBUG
                        assert(g_fjob.state == FIBER_STATE_NONE);
#endif
                        g_fjob.code = 0;
                    } else if (nc->result == KHUI_NC_RESULT_GET_CREDS) {
                        khui_cw_sync_prompt_values(nc);
                        g_fjob.command = FIBER_CMD_CONTINUE;
                        SwitchToFiber(k5_kinit_fiber);

                        /* We get back here once the fiber finishes
                           processing */
                    }
#ifdef DEBUG
                    else {
                        assert(FALSE);
                    }
#endif
                } else {
                    /* we weren't in a KINIT state */
                    if (nc->result == KHUI_NC_RESULT_CANCEL) {
                        /* nothing to report */
                        g_fjob.code = 0;
                    } else if (nc->result == KHUI_NC_RESULT_GET_CREDS) {
                        /* g_fjob.code should have the result of the
                           last kinit attempt.  We should leave it
                           as-is */
                    }
#ifdef DEBUG
                    else {
                        /* unknown result */
                        assert(FALSE);
                    }
#endif
                }

                /* special case: if there was no password entered, and
                   if there is a valid TGT we allow the credential
                   acquisition to go through */
                if (g_fjob.state == FIBER_STATE_NONE &&
                    g_fjob.code &&
                    g_fjob.null_password &&

                    (nc->n_identities == 0 ||
                     nc->identities[0] == NULL ||
                     KHM_SUCCEEDED(kcdb_credset_find_filtered
                                 (NULL,
                                  -1,
                                  k5_find_tgt_filter,
                                  nc->identities[0],
                                  NULL,
                                  NULL))))
                    g_fjob.code = 0;


                if(g_fjob.code != 0) {
                    wchar_t tbuf[1024];
                    DWORD suggestion;
                    kherr_suggestion suggest_code;

                    khm_err_describe(g_fjob.code, tbuf, sizeof(tbuf),
                                     &suggestion, &suggest_code);

                    _report_cs0(KHERR_ERROR, tbuf);
                    if (suggestion != 0)
                        _suggest_mr(suggestion, suggest_code);

                    _resolve();

                    r = KHUI_NC_RESPONSE_FAILED;

                    if (suggest_code == KHERR_SUGGEST_RETRY) {
                        r |= KHUI_NC_RESPONSE_NOEXIT |
                            KHUI_NC_RESPONSE_PENDING;
                    }

#ifdef DEBUG
                    assert(g_fjob.state == FIBER_STATE_NONE);
#endif

                } else if (nc->result == KHUI_NC_RESULT_GET_CREDS &&
                           g_fjob.state == FIBER_STATE_NONE) {
                    khm_handle sp = NULL;
                    khm_handle ep = NULL;
                    krb5_context ctx = NULL;
                    wchar_t * wbuf;
                    wchar_t * idname;
                    wchar_t * atsign;
                    khm_size cb;
                    khm_size cb_ms;
                    khm_int32 rv;

                    r = KHUI_NC_RESPONSE_SUCCESS |
                        KHUI_NC_RESPONSE_EXIT;

                    /* if we successfully obtained credentials, we
                       should save the current settings in the
                       identity config space */

                    assert(nc->n_identities > 0);
                    assert(nc->identities[0]);

                    if(KHM_SUCCEEDED
                       (kcdb_identity_get_config(nc->identities[0],
                                                 KHM_FLAG_CREATE,
                                                 &sp)) &&
                       KHM_SUCCEEDED
                       (khc_open_space(sp, CSNAME_KRB5CRED, 
                                       KHM_FLAG_CREATE, &ep))) {
                        k5_write_dlg_params(ep, d);
                    }

                    if(ep != NULL)
                        khc_close_space(ep);
                    if(sp != NULL)
                        khc_close_space(sp);

                    /* We should also quickly refresh the credentials
                       so that the identity flags and ccache
                       properties reflect the current state of
                       affairs.  This has to be done here so that
                       other credentials providers which depend on
                       Krb5 can properly find the initial creds to
                       obtain their respective creds. */

                    khm_krb5_list_tickets(&ctx);

                    /* also add the principal and the realm in to the
                       LRU lists */
                    rv = kcdb_identity_get_name(nc->identities[0],
                                                NULL,
                                                &cb);
                    assert(rv == KHM_ERROR_TOO_LONG);

                    idname = malloc(cb);
                    assert(idname);

                    rv = kcdb_identity_get_name(nc->identities[0],
                                                idname,
                                                &cb);
                    assert(KHM_SUCCEEDED(rv));

                    rv = khc_read_multi_string(csp_params,
                                               L"LRUPrincipals",
                                               NULL,
                                               &cb_ms);
                    if (rv != KHM_ERROR_TOO_LONG)
                        cb_ms = cb + sizeof(wchar_t);
                    else
                        cb_ms += cb + sizeof(wchar_t);

                    wbuf = malloc(cb_ms);
                    assert(wbuf);

                    cb = cb_ms;

                    if (rv == KHM_ERROR_TOO_LONG) {
                        rv = khc_read_multi_string(csp_params,
                                                   L"LRUPrincipals",
                                                   wbuf,
                                                   &cb);
                        assert(KHM_SUCCEEDED(rv));

                        if (multi_string_find(wbuf,
                                                  idname,
                                                  KHM_CASE_SENSITIVE) 
                            != NULL)
                            /* it's already there */
                            goto _add_realm_to_LRU;
                    } else {
                        multi_string_init(wbuf, cb_ms);
                    }

                    cb = cb_ms;
                    rv = multi_string_prepend(wbuf, &cb, idname);
                    assert(KHM_SUCCEEDED(rv));

                    rv = khc_write_multi_string(csp_params,
                                                L"LRUPrincipals",
                                                wbuf);

                _add_realm_to_LRU:

                    atsign = wcschr(idname, L'@');
                    assert(atsign != NULL);

                    atsign++;
                    assert(*atsign != L'\0');

                    cb = cb_ms;
                    rv = khc_read_multi_string(csp_params,
                                               L"LRURealms",
                                               wbuf,
                                               &cb);

                    if (rv == KHM_ERROR_TOO_LONG) {
                        free(wbuf);
                        wbuf = malloc(cb);
                        assert(wbuf);

                        cb_ms = cb;

                        rv = khc_read_multi_string(csp_params,
                                                   L"LRURealms",
                                                   wbuf,
                                                   &cb);

                        assert(KHM_SUCCEEDED(rv));
                    } else if (rv == KHM_ERROR_SUCCESS) {
                        if (multi_string_find(wbuf,
                                                  atsign,
                                                  KHM_CASE_SENSITIVE)
                            != NULL)
                            goto _done_with_LRU;
                    } else {
                        multi_string_init(wbuf, cb_ms);
                    }

                    cb = cb_ms;
                    rv = multi_string_prepend(wbuf,
                                                  &cb,
                                                  atsign);

                    if (rv == KHM_ERROR_TOO_LONG) {
                        wbuf = realloc(wbuf, cb);

                        rv = multi_string_prepend(wbuf,
                                                      &cb,
                                                      atsign);

                        assert(KHM_SUCCEEDED(rv));
                    }

                    rv = khc_write_multi_string(csp_params,
                                                L"LRURealms",
                                                wbuf);
                    assert(KHM_SUCCEEDED(rv));

                _done_with_LRU:
                    
                    if (ctx != NULL)
                        pkrb5_free_context(ctx);

                    if (idname)
                        free(idname);

                    if (wbuf)
                        free(wbuf);
                } else if (g_fjob.state == FIBER_STATE_NONE) {
                    /* the user cancelled the operation */
                    r = KHUI_NC_RESPONSE_EXIT | 
                        KHUI_NC_RESPONSE_SUCCESS;
                }

                if(g_fjob.state == FIBER_STATE_NONE) {
                    khui_cw_set_response(nc, credtype_id_krb5, r);

                    if (r & KHUI_NC_RESPONSE_NOEXIT) {
                        /* if we are retrying the call, we should
                           restart the kinit fiber */
#ifdef DEBUG
                        assert(r & KHUI_NC_RESPONSE_PENDING);
#endif

                        k5_prep_kinit_job(nc);
                        SwitchToFiber(k5_kinit_fiber);
                    } else {
                        /* free up the fiber data fields. */
                        k5_free_kinit_job();
                    }
                } else {
                    khui_cw_set_response(nc, credtype_id_krb5,
                                         KHUI_NC_RESPONSE_NOEXIT | 
                                         KHUI_NC_RESPONSE_PENDING | r);
                }

                _end_task();
            } else if (nc->subtype == KMSG_CRED_RENEW_CREDS) {

                _begin_task(0);
                _report_mr0(KHERR_NONE, MSG_CTX_RENEW_CREDS);
                _describe();

                if (nc->ctx.scope == KHUI_SCOPE_IDENT ||
                    (nc->ctx.scope == KHUI_SCOPE_CREDTYPE &&
                     nc->ctx.cred_type == credtype_id_krb5)) {
                    int code;

                    if (nc->ctx.identity != 0)
                        code = khm_krb5_renew(nc->ctx.identity);
                    else
                        code = 1; /* it just has to be non-zero */

                    if (code == 0) {
                        khui_cw_set_response(nc, credtype_id_krb5, 
                                             KHUI_NC_RESPONSE_EXIT | 
                                             KHUI_NC_RESPONSE_SUCCESS);
                    } else if (nc->ctx.identity == 0) {

                        _report_mr0(KHERR_ERROR, MSG_ERR_NO_IDENTITY);

                        khui_cw_set_response(nc, credtype_id_krb5, 
                                             KHUI_NC_RESPONSE_EXIT | 
                                             KHUI_NC_RESPONSE_FAILED);
                    } else {
                        wchar_t tbuf[1024];
                        DWORD suggestion;
                        kherr_suggestion sug_id;

                        khm_err_describe(code, tbuf, sizeof(tbuf),
                                         &suggestion, &sug_id);

                        _report_cs0(KHERR_ERROR, tbuf);
                        if (suggestion)
                            _suggest_mr(suggestion, sug_id);

                        _resolve();

                        khui_cw_set_response(nc, credtype_id_krb5, 
                                             ((sug_id == KHERR_SUGGEST_RETRY)?KHUI_NC_RESPONSE_NOEXIT:KHUI_NC_RESPONSE_EXIT) |
                                             KHUI_NC_RESPONSE_FAILED);
                    }
                } else {
                    khui_cw_set_response(nc, credtype_id_krb5, 
                                         KHUI_NC_RESPONSE_EXIT | 
                                         KHUI_NC_RESPONSE_SUCCESS);
                }

                _end_task();
            } else if (nc->subtype == KMSG_CRED_PASSWORD &&
                       nc->result == KHUI_NC_RESULT_GET_CREDS) {

                _begin_task(0);
                _report_mr0(KHERR_NONE, MSG_CTX_PASSWD);
                _describe();

                khui_cw_lock_nc(nc);

                if (nc->n_identities == 0 ||
                    nc->identities[0] == NULL) {
                    _report_mr0(KHERR_ERROR, MSG_PWD_NO_IDENTITY);
                    _suggest_mr(MSG_PWD_S_NO_IDENTITY, KHERR_SUGGEST_RETRY);

                    khui_cw_set_response(nc, credtype_id_krb5,
                                         KHUI_NC_RESPONSE_FAILED |
                                         KHUI_NC_RESPONSE_NOEXIT);
                } else {
                    wchar_t   widname[KCDB_IDENT_MAXCCH_NAME];
                    char      idname[KCDB_IDENT_MAXCCH_NAME];
                    wchar_t   wpwd[KHUI_MAXCCH_PASSWORD];
                    char      pwd[KHUI_MAXCCH_PASSWORD];
                    wchar_t   wnpwd[KHUI_MAXCCH_PASSWORD];
                    char      npwd[KHUI_MAXCCH_PASSWORD];
                    wchar_t   wnpwd2[KHUI_MAXCCH_PASSWORD];
                    wchar_t * wresult;
                    char    * result;
                    khm_size n_prompts = 0;
                    khm_size cb;
                    khm_int32 rv = KHM_ERROR_SUCCESS;
                    long code = 0;
                    khm_handle ident;

                    khui_cw_get_prompt_count(nc, &n_prompts);
                    assert(n_prompts == 3);

                    ident = nc->identities[0];
                    cb = sizeof(widname);
                    rv = kcdb_identity_get_name(ident, widname, &cb);
                    if (KHM_FAILED(rv)) {
#ifdef DEBUG
                        assert(FALSE);
#endif
                        _report_mr0(KHERR_ERROR, MSG_PWD_UNKNOWN);
                        goto _pwd_exit;
                    }

                    cb = sizeof(wpwd);
                    rv = khui_cw_get_prompt_value(nc, 0, wpwd, &cb);
                    if (KHM_FAILED(rv)) {
#ifdef DEBUG
                        assert(FALSE);
#endif
                        _report_mr0(KHERR_ERROR, MSG_PWD_UNKNOWN);
                        goto _pwd_exit;
                    }

                    cb = sizeof(wnpwd);
                    rv = khui_cw_get_prompt_value(nc, 1, wnpwd, &cb);
                    if (KHM_FAILED(rv)) {
#ifdef DEBUG
                        assert(FALSE);
#endif
                        _report_mr0(KHERR_ERROR, MSG_PWD_UNKNOWN);
                        goto _pwd_exit;
                    }

                    cb = sizeof(wnpwd2);
                    rv = khui_cw_get_prompt_value(nc, 2, wnpwd2, &cb);
                    if (KHM_FAILED(rv)) {
#ifdef DEBUG
                        assert(FALSE);
#endif
                        _report_mr0(KHERR_ERROR, MSG_PWD_UNKNOWN);
                        goto _pwd_exit;
                    }

                    if (wcscmp(wnpwd, wnpwd2)) {
                        rv = KHM_ERROR_INVALID_PARM;
                        _report_mr0(KHERR_ERROR, MSG_PWD_NOT_SAME);
                        _suggest_mr(MSG_PWD_S_NOT_SAME, KHERR_SUGGEST_INTERACT);
                        goto _pwd_exit;
                    }

                    if (!wcscmp(wpwd, wnpwd)) {
                        rv = KHM_ERROR_INVALID_PARM;
                        _report_mr0(KHERR_ERROR, MSG_PWD_SAME);
                        _suggest_mr(MSG_PWD_S_SAME, KHERR_SUGGEST_INTERACT);
                        goto _pwd_exit;
                    }

                    UnicodeStrToAnsi(idname, sizeof(idname), widname);
                    UnicodeStrToAnsi(pwd, sizeof(pwd), wpwd);
                    UnicodeStrToAnsi(npwd, sizeof(npwd), wnpwd);

                    result = NULL;

                    code = khm_krb5_changepwd(idname,
                                              pwd,
                                              npwd,
                                              &result);

                    if (code)
                        rv = KHM_ERROR_UNKNOWN;

                    /* result is only set when code != 0 */
                    if (code && result) {
                        size_t len;

                        StringCchLengthA(result, KHERR_MAXCCH_STRING,
                                         &len);
                        wresult = malloc((len + 1) * sizeof(wchar_t));
#ifdef DEBUG
                        assert(wresult);
#endif
                        AnsiStrToUnicode(wresult, (len + 1) * sizeof(wchar_t),
                                         result);

                        _report_cs1(KHERR_ERROR, L"%1!s!", _cstr(wresult));
                        _resolve();

                        free(result);
                        free(wresult);

                        /* leave wresult.  It will get freed when the
                           reported event is freed. */

                        /* we don't need to report anything more */
                        code = 0;
                    }

                _pwd_exit:
                    if (KHM_FAILED(rv)) {
                        if (code) {
                            wchar_t tbuf[1024];
                            DWORD suggestion;
                            kherr_suggestion sug_id;

                            khm_err_describe(code, tbuf, sizeof(tbuf),
                                             &suggestion, &sug_id);
                            _report_cs0(KHERR_ERROR, tbuf);

                            if (suggestion)
                                _suggest_mr(suggestion, sug_id);

                            _resolve();
                        }

                        khui_cw_set_response(nc, credtype_id_krb5, 
                                             KHUI_NC_RESPONSE_NOEXIT|
                                             KHUI_NC_RESPONSE_FAILED);
                    } else {
                        khui_cw_set_response(nc, credtype_id_krb5,
                                             KHUI_NC_RESPONSE_SUCCESS |
                                             KHUI_NC_RESPONSE_EXIT);
                    }
                }

                khui_cw_unlock_nc(nc);

                _end_task();
            } /* KMSG_CRED_PASSWORD */
        }
        break;

    case KMSG_CRED_END:
        {
            khui_new_creds * nc;
            khui_new_creds_by_type * nct;

            nc = (khui_new_creds *) vparam;
            khui_cw_find_type(nc, credtype_id_krb5, &nct);

            if(!nct)
                break;

            khui_cw_del_type(nc, credtype_id_krb5);
    
            if(nct->name)
                free(nct->name);

            free(nct);
        }
        break;

    case KMSG_CRED_IMPORT:
        {
            khm_krb5_ms2mit(TRUE);
        }
        break;
    }

    return rv;
}
