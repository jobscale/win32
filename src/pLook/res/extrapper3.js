/*
 * extrapper.js - JavaScript for ExTrapper
 * (c) 2006-2010 Plustar Co. Ltd.
 * All rights reserved.
 */

// ----------------------------------------------------------------------
// constants
//
var ICON_PATH        = 'lib/images/icons';
var HELPPAGE_URL     = 'http://www.plustar.jp/trapper/help.php?id=';
var CONTACT_URL      = 'http://www.plustar.jp/contact/report.php?p=ext_res&t=';
var DEFAULT_INTERVAL = 30;


// ----------------------------------------------------------------------
// ヘルプメッセージ一覧
// ■プロセス停止エラー //////////////////////////////
// ID：1
//  procname + 'は停止できません。<br>停止するとシステムが不安定になります',
// ■禁止リスト追加エラー ////////////////////////////
// ID：2
// '{0}は起動禁止リストに追加できません。'
// ■スクリーンショット取得エラー //////////////////////
// ID：3
// 'スクリーンショットの取得に失敗しました。<br />電源が入っていないか、ログインしていない可能性があります。'
// ■スクリーンショットパスワード //////////////////////
// ID：4
//  パスワードを入力してください。
// ■アクション時のPC未選択 //////////////////////////
// ID：5
// act + 'するPCを選択してください。<br>',
// ■デバッカを起動してExtrapperを起動 ///////////////
// ID：7
//'FireBugを無効にしてからお使いください。',
// ■ リモート起動 ///////////////////////////////////
// ID：8
//'起動処理要求を送信しました<br>暫くすると起動します<br>',
// ■ダミー 警告 /////////////////////////////////////
//'同期システムは未契約です。<br><a href="'+HELPPAGE_URL+'98" target="_blank">詳細情報...</a>',
//'未成年コンテンツフィルタリングは未契約です。<br><a href="'+HELPPAGE_URL+'99" target="_blank">詳細情報...</a>',

// ----------------------------------------------------------------------
// init Ext
//
Ext.BLANK_IMAGE_URL = 'lib/images/s.gif';
Ext.QuickTips.init();

// ----------------------------------------------------------------------
// global variable
//
var G = {
  main_wnd: {
    grid           : null,
    proc_count     : 0,
    conn_count     : 0
  },
  proc_dlg          : null,
  sshot_dlg         : null,
  opt_dlg           : null,
  lic_dlg           : null,
  pwd_prompt        : null,
  log_dlg           : null,
  net_dlg           : null,
  amachine_dlg      : null,
  machine_dlg       : null,
  install_dlg       : null,
  url_dlg           : null,
  transcope_dlg     : null,
  wakeup_dlg        : null,
  updatehistory_dlg : null
};

// ----------------------------------------------------------------------
// build functions
//
function build_license_dialog() {
  if (!G.lic_dlg) {

    var dlg_close = function() {
      G.lic_dlg.hide();
    };

    var dlg = new Ext.Window( {
      title         : 'ライセンス情報',
      iconCls       : 'btn-key',
      applyTo       : 'lic-dlg',
      layout        : 'fit',
      animateTarget : 'act-btn-help',
      autoScroll    : false,
      autoCreate    : true,
      resizable     : false,
      bodyBorder    : false,
      closeAction   : 'hide',
      closable      : false,
      modal         : true,
      plain         : true,
      height        : 350,
      width         : 450,
      proxyDrag     : true,
      shadow        : false,
      shadowOffset  : 10,
      border        : false,
      html          : '',
      items       : [
      ],
      buttons       : [{
        text        : '閉じる',
        handler     : function() {
          dlg_close();
        }
      }],
      keys          : [{
        key         : Ext.EventObject.ESC,
        handler     : function() {
          dlg_close();
        }
      }]
    });

    dlg.body.on({
      click: { stopEvent: true, delegate: 'a', fn: handler.open_window }
    });

    G.lic_dlg = dlg;
  }
  return G.lic_dlg;
}

function build_log_dialog()
{
  if (!G.log_dlg) {
    var ds = new Ext.data.JsonStore({
      url        : '/log',
      storeId    : 'dy',
      sortInfo   : {field:'dy', direction:'DESC'},
      fields     : [
        {name: 'dy', type: 'string'},
        {name: 'hn', type: 'string'},
        {name: 'ip', type: 'string'},
        {name: 'sn', type: 'string'},
        {name: 'kg', type: 'string'}
      ]
    });

    // process list dialog's column
    var cm = new Ext.grid.ColumnModel([{
      id: 'col-dy',
      header: '停止日',
      dataIndex: 'dy',
      width: 150,
      align: 'right',
      sortable: true
    },{
      header: 'マシン名',
      dataIndex: 'hn',
      width: 90,
      align: 'left',
      sortable: true
    },{
      header: 'IP',
      dataIndex: 'ip',
      width: 90,
      align: 'left',
      sortable: true
    },{
      header: 'ソフト名',
      dataIndex: 'sn',
      width: 150,
      align: 'left',
      sortable: true
    },{
      header: '停止',
      dataIndex: 'kg',
      width: 20,
      align: 'left',
      sortable: true,
      renderer  : function(x) {
        switch(x) {
        // "pc_stop" 個別PCの自動停止 (pc) ←手動停止もこれ
        case 'pc_stop':
          return '<img src="'+ICON_PATH+'/ue.png" alt="自動" title="自動" />';
        // "ad_stop" 管理設定の自動停止 (admin)
        case 'ad_stop':
          return '<img src="'+ICON_PATH+'/sy.png" alt="手動" title="手動" />';
        // "se_stop" サーバ設定の自動停止 (server)
        default:
          return '<img src="'+ICON_PATH+'/de.png" alt="自動" title="自動" />';
        }
      }
    }]);

    cm.defaultSortable = true;

    // build process list dialog
    var grid = new Ext.grid.GridPanel( {
      region           : 'center',
      border           : false,
      ds               : ds,
      cm               : cm,
      autoExpandColumn : 'dy',
      viewConfig       : {forceFit: true},
      height           : 340,
      width            : 495,
      loadMask         : {msg:'プロセスデータ取得中...'},
      autoScroll       : true,
      layout           : 'fit',
      stripeRows       : true,
      switch_target    : function(target) {
        ds.load();
      }
    });

    var dlg_close = function() {
      G.log_dlg.hide();
      G.log_dlg = null;
    };

    var dlg = new Ext.Window({
      animateTarget : 'act-btn-tools',
      iconCls       : 'btn-log',
      layout        : 'fit',
      border        : false,
      plain         : true,
      closable      : false,
      resizable     : false,
      height        : 410,
      width         : 550,
      modal         : true,
      proxyDrag     : true,
      shadow        : true,
      shadowOffset  : 10,
      constrain     : true,
      items         : [grid],
      tbar          : [{
        cls: 'x-btn-text-icon',
        text: '停止プロセス検索',
        icon: ICON_PATH+'/zm.png',
        tooltip: '停止プロセス名の検索を行います',
        handler: onStopProcessSearch,
        scope: this
      }],
      buttons   : [{
        text    : '閉じる',
        handler : function() {
          dlg_close();
        }
      }],
      keys: [{
        key     : Ext.EventObject.ESC,
        handler : function() {
          dlg_close();
        }
      }]
    });

    // set event handler for grid rows
    grid.on('rowdblclick', function(grid, idx, e) {
      window.open('http://www.plustar.jp/proc.php?q=' + grid.getStore().getAt(idx).get('sn'),'google_search');
    });

    grid.switch_target();
    dlg.grid  = grid;
    dlg.ds    = ds;
    G.log_dlg = dlg;
  }
  else
  {
    G.log_dlg = null;
  }
  return G.log_dlg;
}

function onStopProcessSearch() {
  var selected = G.log_dlg.grid.getSelectionModel().getSelected();
  if (!selected) {
    Ext.MessageBox.show({
       title   : 'メッセージ',
       msg     : '検索するプロセスを選択してください。',
       width   : 300,
       buttons : Ext.MessageBox.OK,
       icon    : Ext.MessageBox.INFO
    });
    return;
  }
  var procname = selected.get('sn');
  window.open('http://www.plustar.jp/proc.php?q=' + procname, 'google_search');
}


function build_process_dialog(hostname,macaddr) {
  if (!G.proc_dlg) {
    var ds = new Ext.data.JsonStore({
      url        : '/process',
      autoLoad   : true,
      baseParams : { 'macaddr': macaddr },
      storeId    : 'pid',
      sortInfo   : {field:'pid', direction:'ASC'},
      fields     : [
        {name: 'pid',       type: 'int'   },
        {name: 'sid',       type: 'int'   },
        {name: 'procname',  type: 'string'},
        {name: 'begin',     type: 'date'  },
        {name: 'time_span', type: 'string'}
      ]
    });

    // process list dialog's column
    var cm = new Ext.grid.ColumnModel([
    new Ext.grid.RowNumberer(),
    {
      id        : 'col-procname',
      header    : 'イメージ名',
      dataIndex : 'procname',
      width     : 100,
      sortable  : true
    },{
      header    : 'PID',
      dataIndex : 'pid',
      width     : 35,
      align     : 'right',
      sortable  : true
    },{
      header    : 'ID',
      dataIndex : 'sid',
      width     : 20,
      align     : 'right',
      sortable  : true
    },{
      header    : '起動日時',
      dataIndex : 'begin',
      width     : 110,
      renderer  : Ext.util.Format.dateRenderer('Y/m/d H:i:s'),
      sortable  : true
    },{
      header    : '経過時間',
      dataIndex : 'time_span',
      width     : 60,
      align     : 'right',
      sortable  : true
    }]);
    cm.defaultSortable = true;

    // build process list dialog
    var grid = new Ext.grid.GridPanel( {
      region           : 'center',
      border           : false,
      ds               : ds,
      cm               : cm,
      autoExpandColumn : 'pid',
      viewConfig       : {forceFit: true},
      height           : 340,
      width            : 455,
      loadMask         : {msg:'プロセスデータ取得中...'},
      autoScroll       : true,
      layout           : 'fit',
      stripeRows       : true,
      switch_target    : function(target) {
        ds.load();
      }
    });

    var dlg_close = function() {
      G.proc_dlg.hide();
      G.proc_dlg = null;
    };

    var dlg = new Ext.Window({
      animateTarget : 'act-btn-procs',
      iconCls       : 'btn-proc',
      layout        : 'fit',
      border        : false,
      plain         : true,
      closable      : false,
      resizable     : false,
      height        : 410,
      width         : 450,
      modal         : true,
      proxyDrag     : true,
      shadow        : true,
      shadowOffset  : 10,
      constrain     : true,
      items         : [grid],
      tbar          : [{
        id      : 'act-btn-proc',
        cls     : 'x-btn-text-icon',
        text    : '停止',
        icon    : ICON_PATH+'/st.png',
        tooltip : '選択したプロセスを停止します',
        handler : onProcessStop,
        scope: this
      },{
        cls     : 'x-btn-text-icon',
        text    : '更新',
        icon    : ICON_PATH+'/ah.png',
        tooltip : 'プロセス一覧を更新します',
        handler : function() {
          ds.load();
        },
        scope: this
      }, '-' ,{
        cls: 'x-btn-text-icon',
        text: 'プロセス検索',
        icon: ICON_PATH+'/zm.png',
        tooltip: 'プロセス名の検索を行います',
        handler: onProcessSearch,
        scope: this
      }],
      buttons   : [{
        text    : '閉じる',
        handler : function() {
          dlg_close();
        }
      }],
      keys: [{
        key     : Ext.EventObject.ESC,
        handler : function() {
          dlg_close();
        }
      }]
    });

    // set event handler for grid rows
    grid.on('rowdblclick', function(grid, idx, e) {
      window.open('http://www.plustar.jp/proc.php?q=' + grid.getStore().getAt(idx).get('procname'),'google_search');
    });

    //grid.switch_target();
    dlg.grid = grid;
    dlg.ds   = ds;
    G.proc_dlg = dlg;
  }
  else
  {
    G.proc_dlg = null;
  }
  return G.proc_dlg;
}

function onProcessStop() {
  var selected = G.proc_dlg.grid.getSelectionModel().getSelected();
  if (!selected) {
    Ext.MessageBox.show({
       title   : 'メッセージ',
       msg     : '停止するプロセスを選択してください。',
       width   : 300,
       buttons : Ext.MessageBox.OK,
       icon    : Ext.MessageBox.INFO
    });
    return;
  }
  
  var procname = selected.get('procname');
  var p = procname.toLowerCase();

  if(p == '[system process]'|| p == 'csrss.exe'|| p == 'lsass.exe'|| p == 'pextrapper.exe'|| p == 'ppower.exe'|| p == 'ptools.exe'|| p == 'services.exe'|| p == 'smss.exe'|| p == 'spoolsv.exe'|| p == 'svchost.exe'|| p == 'system'|| p == 'winlogon.exe' || p == 'pcontroler.exe' || p == 'pcheckwindow.exe')
  {
    Ext.MessageBox.show({
       title   : '警告',
       msg     : procname + 'は停止できません。<br>停止するとシステムが不安定になります。<br><a href="'+HELPPAGE_URL+'1&procname='+procname+'" target="_blank">エラー詳細情報...</a>',
       width   : 300,
       buttons : Ext.MessageBox.OK,
       icon    : Ext.MessageBox.ERROR
    });
    return;
  }

  Ext.MessageBox.confirm(
    '確認',
    'プロセスを終了すると、データが失われたり、システムが不安定になったりするなどの、予期しない結果になることがあります。<br /><br />プロセスを終了する前に、状態またはデータを保存するかどうかの確認メッセージは表示されません。<br /><br />プロセスを終了しますか?',
    function(btn) {
      if (btn == 'yes') {
        G.proc_dlg.ds.load({
          params: {procname: selected.get('procname')}
        });
      }
    }
  );
}

function onProcessSearch() {
  var selected = G.proc_dlg.grid.getSelectionModel().getSelected();
  if (!selected) {
    Ext.MessageBox.show({
       title   : 'メッセージ',
       msg     : '検索するプロセスを選択してください。',
       width   : 300,
       buttons : Ext.MessageBox.OK,
       icon    : Ext.MessageBox.INFO
    });
    return;
  }
  var procname = selected.get('procname');
  window.open('http://www.plustar.jp/proc.php?q=' + procname, 'google_search');
}

function build_screenshot_password_prompt() {
  if (!G.pwd_prompt) {
    var password_box = '<span class="ext-mb-text">パスワードを入力してください。</span><br /><input type="password" class="ext-mb-input" id="sshot_pwd"/><br>&nbsp;&nbsp;<a href="'+HELPPAGE_URL+'4" target="_blank">詳細情報...</a>';

    var dlg = new Ext.Window({
      id          : 'sshot-prompt',
      title       : '認証',
      iconCls     : 'btn-sshot',
      closeAction : 'hide',
      closable    : false,
      shadow      : true,
      draggable   : true,
      resizable   : false,
      fixedcenter : true,
      collapsible : false,
      shim        : true,
      modal       : true,
      width       : 300,
      height      : 135,
      layout      : 'fit',
      buttonAlign :'center',
      constrain   : true,
      items       : [{
          html    : password_box
      }],
      buttons     : [{
        text      : 'OK',
        handler   : function() {
          if(dlg.isVisible()) {
            pwd = Ext.get('sshot_pwd');
            G.pwd_prompt.callback(pwd.getValue());
            G.pwd_prompt.hide();
          }
        }
      },{
        text      : 'キャンセル',
        handler: function() {
          G.pwd_prompt.hide();
        }
      }],
      keys: [{
        key       : Ext.EventObject.ESC,
        handler: function() {
         G.pwd_prompt.hide();
         G.pwd_prompt = null;
        }
      },{
        key       : Ext.EventObject.ENTER,
        handler: function() {
          if(dlg.isVisible()) {
            pwd = Ext.get('sshot_pwd');
            G.pwd_prompt.callback(pwd.getValue());
            G.pwd_prompt.hide();
          }
        }
      }]
    });
    dlg.show();

    G.pwd_prompt = dlg;
  }
  Ext.getDom('sshot_pwd').value = '';
  return G.pwd_prompt;
}

function build_option_dialog() {
  if(!G.opt_dlg) {
    var tabs = new Ext.TabPanel({
        region          : 'center',
        animScroll      : true,
        enableTabScroll : true,
        deferredRender  : false
    });

    var dlg = new Ext.Window({
        applyTo       : 'opt-dlg',
        animateTarget : 'act-btn-tools',
        iconCls       : 'btn-opt',
        closeAction   : 'hide',
        closable      : false,
        width         : 360,
        height        : 400,
        minimizable   : true,
        border        : false,
        plain         : true,
        layout        : 'border',
        modal         : true,
        shadow        : true,
        shadowOffset  : 10,
        autoScroll    : false,
        resizable     : false,
        proxyDrag     : true,
        constrain     : true,
        items         : [
          tabs
        ],
        buttons       : [{
          text        : 'OK',
          handler     : function() {
            tabs.getActiveTab().submit(true);
          }
        },{
          text        : 'キャンセル',
          handler     : function() {
            G.opt_dlg.hide();
          }
        },{
          text        : '適用',
          handler     : function() {
            tabs.getActiveTab().submit();
          }
        }],
        keys          : [{
          key         : Ext.EventObject.ESC,
          handler     : function() {
            G.opt_dlg.hide();
          }
        }]
    });

    dlg.on('minimize', function(){
      dlg.toggleCollapse();
    });

    var suppression_form = build_suppression_grid(dlg);
    suppression_form.submit = function(is_close) {
      if (!suppression_form.updated) {
        if (is_close)
        {
           G.opt_dlg.hide();
         }
        return;
      }
      var ds = suppression_form.getStore() ;
      var data = new Array();
      var ignore = '';
      Ext.each(ds.getRange(), function(o, i) {
        var procname = o.get('procname');
        var p = procname.toLowerCase();
        if ((p == 'csrss.exe'||  p == 'lsass.exe'||  p == 'pextrapper.exe'||  p == 'ppower.exe'||  p == 'ptools.exe'||  p == 'services.exe'||  p == 'smss.exe'||  p == 'spoolsv.exe'||  p == 'svchost.exe'||  p == 'system'||  p == 'winlogon.exe' || p == 'pcontroler.exe' || p == 'pcheckwindow.exe')|| (p.length < 7))
        {
          ignore += procname+' ';
        } if (p!=='') {
          data[i] = {procname: procname};
        }
      });

      if (ignore) {
        Ext.MessageBox.show({
           title   : 'メッセージ',
           msg     : String.format('{0}は起動禁止リストに追加できません。<br><a href="'+HELPPAGE_URL+'2&procname={0}" target="_blank">エラー詳細情報...</a>',ignore),
           width   : 300,
           buttons : Ext.MessageBox.OK,
           icon    : Ext.MessageBox.WARNING
        });

        return;
      }

      Ext.MessageBox.confirm(
        '確認',
        '指定したプロセスによっては、データが失われたり、システムが不安定になったりするなどの、予期しない結果になることがあります。<br /><br />プロセスを終了する前に、状態またはデータを保存するかどうかの確認メッセージは表示されません。<br /><br />起動禁止リストを保存しますか?',
        function(btn) {
          if (btn=='yes') {
            Ext.MessageBox.alert(
              'メッセージ',
              '起動禁止リストを保存しました。',
              function() {
                if (is_close) {
                  G.opt_dlg.hide();
                } else {
                  ds.load({params: {data: Ext.encode(data)}});
                  suppression_form.updated = false;
                  dlg.buttons[2].disable();
                }
              }
            );
          }
        }
      );
    };
    suppression_form.on({
      activate: {
        fn: function() {
          suppression_form.getView().refresh(true);
          dlg.buttons[2].setDisabled(!suppression_form.updated);
        }
      },
      deactivate: {
        fn: function() {
          suppression_form.stopEditing();
        }
      }
    });

    var sync_form = build_sync_form(dlg);
    sync_form.submit = function(is_close) {
      if (!sync_form.updated) {
        if (is_close)
        {
           G.opt_dlg.hide();
         }
        return;
      }
      sync_form.form.submit({
        failure: function(form, action) {
          //Ext.MessageBox.alert('Error Message', action.result.errorInfo);
        },
        success: function(form, action) {
          Ext.MessageBox.alert(
            'メッセージ',
            '同期サーバ設定を変更しました。',
            function() {
              if (is_close) {
                G.opt_dlg.hide();
              } else {
                form.updated = false;
                dlg.buttons[2].setDisabled(true);
              }
            }
          );
        }
      });
    };
    sync_form.on({activate: {fn: function() {
        sync_form.load();
        dlg.buttons[2].setDisabled(!sync_form.updated);
    }}});

    var password_form = build_password_form(dlg);
    password_form.submit = function(is_close) {
      if (!password_form.updated) {
        if (is_close)
        {
           G.opt_dlg.hide();
         }
        return;
      }
        password_form.form.submit({
          reset: true,
          scope: this,
          failure: function(form, action) {
            //Ext.MessageBox.alert('Error Message', action.result.errorInfo);
          },
          success: function(form, action) {
            password_form.updated = false;
            Ext.MessageBox.alert(
              'メッセージ',
              'パスワードを変更しました。',
              function() {
                if (is_close) {
                  G.opt_dlg.hide();
                } else {
                  password_form.updated = false;
                  dlg.buttons[2].setDisabled(true);
                }
              }
            );
          }
        });
    };
    password_form.on({
      activate: function() {
        dlg.buttons[2].setDisabled(!password_form.updated);
      }
    });

    var net_form = build_net_form(dlg);
    net_form.submit = function(is_close) {
      if (!net_form.updated) {
        if (is_close)
        {
           G.opt_dlg.hide();
         }
        return;
      }
      net_form.form.submit({
        failure: function(net_form, action) {
          //Ext.MessageBox.alert('Error Message', action.result.errorInfo);
        },
        success: function(net_form, action) {
          Ext.MessageBox.alert(
            'メッセージ',
            'ネットワーク設定を変更しました。',
            function() {
              if (is_close) {
                G.opt_dlg.hide();
              } else {
                net_form.updated = false;
                dlg.buttons[2].setDisabled(true);
              }
            }
          );
        }
      });
    };
    net_form.on({activate: {fn: function() {
        net_form.load();
        dlg.buttons[2].setDisabled(!net_form.updated);
    }}});

    var misc_form = build_misc_form(dlg);
    misc_form.submit = function(is_close) {
      if (!misc_form.updated) {
        if (is_close)
        {
           G.opt_dlg.hide();
         }
        return;
      }
      misc_form.form.submit({
        scope  : this,
        failure: function(misc_form, action) {
          //Ext.MessageBox.alert('Error Message', action.result.errorInfo);
        },
        success: function(misc_form, action) {
          Ext.MessageBox.alert(
            'メッセージ',
            '一括設定を禁止リストに反映しました。',
            function() {
              if (is_close) {
                G.opt_dlg.hide();
              } else {
                misc_form.updated = false;
                dlg.buttons[2].setDisabled(true);
                suppression_form.switch_target();
              }
            }
          );
        }
      });
    };
    misc_form.on({activate: {fn: function() {
        misc_form.load();
        dlg.buttons[2].setDisabled(!misc_form.updated);
    }}});

    var notice_form = build_notice_form(dlg);
    notice_form.submit = function(is_close) {
      if (!notice_form.updated) {
        if (is_close)
        {
           G.opt_dlg.hide();
         }
        return;
      }
      notice_form.form.submit({
        scope  : this,
        failure: function(notice_form, action) {
          //Ext.MessageBox.alert('Error Message', action.result.errorInfo);
        },
        success: function(notice_form, action) {
          Ext.MessageBox.alert(
            'メッセージ',
            '通知設定を保存しました。',
            function() {
              if (is_close) {
                G.opt_dlg.hide();
              } else {
                notice_form.updated = false;
                dlg.buttons[2].setDisabled(true);
              }
            }
          );
        }
      });
    };
    notice_form.on({activate: {fn: function() {
        notice_form.load();
        dlg.buttons[2].setDisabled(!notice_form.updated);
    }}});

    // タブの並び
    tabs.add(suppression_form);
    tabs.add(misc_form);
    tabs.add(sync_form);
    tabs.add(password_form);
    tabs.add(net_form);
    tabs.add(notice_form);

    // 初期状態アクティブなタブ
    tabs.setActiveTab(0);

    G.opt_dlg = dlg;
  }
  return G.opt_dlg;
}

function build_password_form(dlg) {
  if (!dlg.form_pwd) {
    // build form for password
    Ext.apply(Ext.form.VTypes, {
      password: function(value, field) {
        if (field.initialPassField) {
          var new_password = Ext.getCmp(field.initialPassField);
          return (value == new_password.getValue());
        }
        return true;
      },
      passwordText: 'パスワードが一致しません。'
    });   

    var form = new Ext.FormPanel({
      frame            : true,
      header           : false,
      width            : 340,
      title            : 'パスワード',
      id               : 'tabs-password',
      iconCls          : 'tabs-password',
      autoDestroy      : true,
      trackResetOnLoad : true,
      //standardSubmit   : true,
      timeout          : 10,
      url              : '/password',
      method           : 'POST',
      items            : [{
        border : false,
        items  : [{
          items: {
            labelWidth  : 10,
            xtype       : 'fieldset',
            title       : '変更パスワード種別',
            height      : 90,
            defaultType : 'radio',
            items: [{
              fieldLabel     : '',
              boxLabel       : 'ログインパスワード',
              inputValue     : 'login',
              name           : 'password_type',
              labelSeparator : '',
              checked        : true
            },{
              fieldLabel     : '',
              boxLabel       : 'スクリーンショットパスワード',
              inputValue     : 'sshot',
              name           : 'password_type',
              labelSeparator : '',
              checked        : false
            }]
          }
        }]
      },{
        xtype       : 'fieldset',
        labelWidth  : 140,
        title       : 'パスワード設定',
        height      : 120,
        defaultType : 'textfield',
        layout      : 'form',
        items: [{
          fieldLabel    : '現在のパスワード',
          name          : 'old_password',
          id            : 'old_password',
          inputType     : 'password',
          selectOnFocus : true,
          msgTarget     : 'side',
          allowBlank    : false,
          width         : 125
        },{
          fieldLabel    : '新しいパスワード',
          name          : 'new_password',
          id            : 'new_password',
          inputType     : 'password',
          selectOnFocus : true,
          msgTarget     : 'side',
          allowBlank    : false,
          maxLength     : 15,
          minLength     : 5,
          width         : 125
        },{
          fieldLabel       : '新しいパスワードを確認',
          name             : 'cfm_password',
          id               : 'cfm_password',
          inputType        : 'password',
          selectOnFocus    : true,
          msgTarget        : 'side',
          allowBlank       : false,
          maxLength        : 15,
          minLength        : 5,
          vtype            : 'password',
          initialPassField : 'new_password',
          width            : 125
        }]
      }]
    });
    form.render(document.body);

    Ext.getCmp('old_password').on('change',function(f,nv,ov){
      form.updated = true;
      dlg.buttons[2].enable();
    });
    Ext.getCmp('new_password').on('change',function(f,nv,ov){
      form.updated = true;
      dlg.buttons[2].enable();
    });
    Ext.getCmp('cfm_password').on('change',function(f,nv,ov){
      form.updated = true;
      dlg.buttons[2].enable();
    });

    dlg.form_pwd = form;
  }

  return dlg.form_pwd;
}

function build_suppression_grid(dlg) {
  if (!dlg.suppression_grid) {

    var SuppressionItem = Ext.data.Record.create([
//      {name: 'procname', type: 'string', editor: new Ext.form.TextField({})}
      {name: 'procname', type: 'string'}
    ]);

    var cm = new Ext.grid.ColumnModel([{
      id: 'suppression-col-procname',
      header: '起動禁止プロセス',
      dataIndex: 'procname',
      resizable: false,
      width: 200,
      sortable: true,
      editor: new Ext.form.TextField({
        allowBlank: false
      })
//      editor: new Ext.grid.GridEditor(
//        new Ext.form.TextField({
//          allowBlank: false
//      }))
    }]);

    var ds = new Ext.data.JsonStore({
      url      : '/suppression',
      sortInfo : {field:'procname', direction:'ASC'},
      fields   : [{name: 'procname', type: 'string'}]
    });

    ds.on({
      add: function(ds, rec, idx) {
        dlg.suppression_grid.updated = true;
        dlg.buttons[2].enable();
      },
      remove: function(ds, rec, idx) {
        dlg.suppression_grid.updated = true;
        dlg.buttons[2].enable();
      },
      update: function(ds, rec, op) {
        dlg.suppression_grid.updated = true;
        dlg.buttons[2].enable();
      }
    });

    // use RowEditor for editing
    var editor = new Ext.ux.grid.RowEditor({
        saveText   : '更新',
        cancelText : 'キャンセル'
    });

    // build grid for suppression list
//    var grid = new Ext.grid.EditorGridPanel( {
    var grid = new Ext.grid.GridPanel( {
      frame            : true,
      title            : '起動禁止リスト',
      id               : 'tabs-stop',
      iconCls          : 'tabs-stop',
      ds               : ds,
      cm               : cm,
      autoExpandColumn : 'suppression-col-procname',
      selModel         : new Ext.grid.RowSelectionModel({singleSelect: false}),
      enableColLock    : false,
      enableColumnHide : false,
      frame            : true,
      width            : 340,
      loadMask         : {msg:'禁止リストデータ取得中...'},
      viewConfig       : {forceFit: true},
      plugins          : [editor],
      switch_target    : function(target) {
        ds.load();
      },
      tbar: [{
        cls: 'x-btn-text-icon',
        text: '追加',
        icon: ICON_PATH+'/ad.png',
        tooltip: '項目を追加します',
        handler: function(){
          var i = new SuppressionItem({
            procname: ''
          });
          grid.stopEditing();
          ds.insert(0, i);
          grid.startEditing(0, 0);
        },
        scope: this
      },{
        id: 'act-btn-proc',
        cls: 'x-btn-text-icon',
        text: '削除',
        icon: ICON_PATH+'/de.png',
        tooltip: '選択した項目を削除します',
        handler: function() {
          //var selected = grid.getSelections();
          var selected = grid.getSelectionModel().getSelected();
          if (!selected) {
            Ext.MessageBox.show({
               title   : 'メッセージ',
               msg     : '削除する項目を選択してください。',
               width   : 300,
               buttons : Ext.MessageBox.OK,
               icon    : Ext.MessageBox.INFO
            });
            return;
          }
          Ext.each(selected, function(row, idx) {
            ds.remove(row);
          });
        },
        scope: this
      }],
      viewConfig: {
        forceFit: true
      }
    });
    grid.show();

    grid.switch_target();

    dlg.suppression_grid = grid;
  }
  return dlg.suppression_grid;
}

function build_misc_form(dlg) {
  if (!dlg.form_misc) {
    var form = new Ext.FormPanel({
      frame            : true,
      header           : false,
      title            : '一括禁止設定',
      id               : 'tabs-etc',
      iconCls          : 'tabs-etc',
      width            : 340,
      labelWidth       : 0,
      trackResetOnLoad : true,
      timeout          : 10,
      url              : '/misc',
      method           : 'POST',
      items: [{
        border: false,
        items: [{
          xtype      :'fieldset',
          title      : 'タスクマネージャ系設定',
          autoHeight : true,
          items: [{
            xtype          : 'radiogroup',
            fieldLabel     : '',
            labelSeparator : '',
            labelWidth     : 0,
            items: [{
              boxLabel       : '起動許可',
              inputValue     : 1,
              name           : 'tm_type',
              id             : 'tm_allow'
            },{
              boxLabel       : '起動禁止',
              inputValue     : 0,
              name           : 'tm_type',
              id             : 'tm_deny'
            }]
          }]
        },{
          xtype      :'fieldset',
          title      : 'レジストリエディタ系設定',
          autoHeight : true,
          items: [{
            xtype          : 'radiogroup',
            fieldLabel     : '',
            labelSeparator : '',
            labelWidth     : 0,
            items: [{
              boxLabel       : '起動許可',
              inputValue     : 1,
              name           : 're_type',
              id             : 're_allow'
            },{
              boxLabel       : '起動禁止',
              inputValue     : 0,
              name           : 're_type',
              id             : 're_deny'
            }]
          }]
        },{
          xtype      :'fieldset',
          title      : 'ファイル共有系設定',
          autoHeight : true,
          items: [{
            xtype          : 'radiogroup',
            fieldLabel     : '',
            labelSeparator : '',
            labelWidth     : 0,
            items: [{
              boxLabel       : '起動許可',
              inputValue     : 1,
              name           : 'p2p_type',
              id             : 'p2p_allow'
            },{
              boxLabel       : '起動禁止',
              inputValue     : 0,
              name           : 'p2p_type',
              id             : 'p2p_deny'
            }]
          }]
        }]
      }]
    });

    var store = new Ext.data.JsonStore({
      url: '/misc',
      fields: ['tm_type', 're_type', 'p2p_type']
    });

    store.on('load', function(s, r, o) {
      if (r[0].get('tm_type')) {
        Ext.getCmp('tm_allow').setValue(true);
      } else {
        Ext.getCmp('tm_deny').setValue(true);
      }
      if (r[0].get('re_type')) {
        Ext.getCmp('re_allow').setValue(true);
      } else {
        Ext.getCmp('re_deny').setValue(true);
      }
      if (r[0].get('p2p_type')) {
        Ext.getCmp('p2p_allow').setValue(true);
      } else {
        Ext.getCmp('p2p_deny').setValue(true);
      }
    });
    store.load();
    form.render(document.body);

    Ext.getCmp('tm_allow').on('focus',function(f,nv,ov){
      form.updated = true;dlg.buttons[2].enable();
    });
    Ext.getCmp('tm_deny').on('focus',function(f,nv,ov){
      form.updated = true;dlg.buttons[2].enable();
    });
    Ext.getCmp('re_allow').on('focus',function(f,nv,ov){
      form.updated = true;dlg.buttons[2].enable();
    });
    Ext.getCmp('re_deny').on('focus',function(f,nv,ov){
      form.updated = true;dlg.buttons[2].enable();
    });
    Ext.getCmp('p2p_allow').on('focus',function(f,nv,ov){
      form.updated = true;dlg.buttons[2].enable();
    });
    Ext.getCmp('p2p_deny').on('focus',function(f,nv,ov){
      form.updated = true;dlg.buttons[2].enable();
    });

    dlg.form_misc = form;
  }

  return dlg.form_misc;
}

function build_notice_form(dlg) {
  if (!dlg.form_notice) {
    Ext.apply(Ext.form.VTypes, {
      ismail: function(value) {
        return (value.match(/^([a-zA-Z0-9])+([a-zA-Z0-9\._-])*@([a-zA-Z0-9_-])+([a-zA-Z0-9\._-]+)+$/));
      },
      ismailText: '正しいメールアドレス形式ではありません。<br>例）info@plustar.jp'
    });

    var form = new Ext.FormPanel({
      frame            : true,
      header           : false,
      title            : '通知',
      id               : 'tabs-notice',
      iconCls          : 'tabs-notice',
      width            : 340,
      labelWidth       : 0,
      trackResetOnLoad : true,
      timeout          : 10,
      url              : '/notice',
      method           : 'POST',
      items: [{
        border: false,
        items: [{
          xtype      :'fieldset',
          title      : '起動停止通知メッセージ',
          autoHeight : true,
          items: [{
            xtype          : 'radiogroup',
            fieldLabel     : '',
            labelSeparator : '',
            labelWidth     : 0,
            items: [{
              boxLabel       : '使用する',
              inputValue     : 1,
              name           : 'nc_pop',
              id             : 'pop_allow'
            },{
              boxLabel       : '使用しない',
              inputValue     : 0,
              name           : 'nc_pop',
              id             : 'pop_deny'
            }]
          }]
        },{
          xtype      :'fieldset',
          title      : '起動停止通知メール送信',
          autoHeight : true,
          items: [{
            xtype          : 'radiogroup',
            fieldLabel     : '',
            labelSeparator : '',
            labelWidth     : 0,
            items: [{
              boxLabel       : '使用する',
              inputValue     : 1,
              name           : 'nc_mail',
              id             : 'mail_allow'
            },{
              boxLabel       : '使用しない',
              inputValue     : 0,
              name           : 'nc_mail',
              id             : 'mail_deny'
            }]
          }]
        },{
          xtype       : 'fieldset',
          labelWidth  : 120,
          title       : '通知メール送信先アドレス',
          height      : 60,
          defaultType : 'textfield',
          layout      : 'form',
          items: [{
            fieldLabel    : 'アドレス',
            name          : 'nc_mailadd',
            id            : 'nc_mailadd',
            emptyText     : 'trapper@plustar.jp',
            selectOnFocus : true,
            msgTarget     : 'side',
            maxLength     : 55,
            minLength     : 7,
            allowBlank    : true,
            vtype         : 'ismail',
            width         : 155
          }]
        },{
          xtype      :'fieldset',
          title      : 'アラート音源設定',
          autoHeight : true,
          items: [{
            xtype          : 'radiogroup',
            fieldLabel     : '',
            labelSeparator : '',
            labelWidth     : 0,
            items: [{
              boxLabel       : 'Wave',
              inputValue     : 0,
              name           : 'alert_type',
              id             : 'alert_wav'
            },{
              boxLabel       : 'Beep',
              inputValue     : 1,
              name           : 'alert_type',
              id             : 'alert_beep'
            }]
          }]
        }]
      }]
    });

    var store = new Ext.data.JsonStore({
      url: '/notice',
      fields: ['nc_pop', 'nc_mail', 'nc_mailadd', 'alert_type']
    });

    store.on('load', function(s, r, o) {
      if (r[0].get('nc_pop')) {
        Ext.getCmp('pop_allow').setValue(true);
      } else {
        Ext.getCmp('pop_deny').setValue(true);
      }
      if (r[0].get('nc_mail')) {
        Ext.getCmp('mail_allow').setValue(true);
      } else {
        Ext.getCmp('mail_deny').setValue(true);
      }
      var to_mailadd = r[0].get('nc_mailadd');
      if (to_mailadd !== '') {
        Ext.getCmp('nc_mailadd').setValue(to_mailadd);
      } else {
        Ext.getCmp('nc_mailadd').setValue('');
      }
      if (r[0].get('alert_type')) {
        Ext.getCmp('alert_beep').setValue(true);
      } else {
        Ext.getCmp('alert_wav').setValue(true);
      }
    });
    store.load();
    form.render(document.body);

    Ext.getCmp('pop_allow').on('focus',function(f,nv,ov){
      form.updated = true;dlg.buttons[2].enable();
    });
    Ext.getCmp('pop_deny').on('focus',function(f,nv,ov){
      form.updated = true;dlg.buttons[2].enable();
    });
    Ext.getCmp('mail_allow').on('focus',function(f,nv,ov){
      form.updated = true;dlg.buttons[2].enable();
    });
    Ext.getCmp('mail_deny').on('focus',function(f,nv,ov){
      form.updated = true;dlg.buttons[2].enable();
    });
    Ext.getCmp('nc_mailadd').on('change',function(f,nv,ov){
      form.updated = true;dlg.buttons[2].enable();
    });
    Ext.getCmp('alert_wav').on('focus',function(f,nv,ov){
      form.updated = true;dlg.buttons[2].enable();
    });
    Ext.getCmp('alert_beep').on('focus',function(f,nv,ov){
      form.updated = true;dlg.buttons[2].enable();
    });

    dlg.form_notice = form;
  }

  return dlg.form_notice;
}

function build_net_form(dlg) {
  if (!dlg.form_net) {
    var form = new Ext.FormPanel({
      frame            : true,
      header           : false,
      title            : 'ネットワーク設定',
      id               : 'tabs-net',
      iconCls          : 'tabs-net',
      width            : 340,
      labelWidth       : 0,
      trackResetOnLoad : true,
      timeout          : 10,
      url              : '/net',
      method           : 'POST',
      items: [{
        labelWidth : 0,
        border: false,
        items: [{
          xtype      :'fieldset',
          title      : '回線切断監視設定',
          autoHeight : true,
          items: [{
            xtype          : 'radiogroup',
            fieldLabel     : '',
            labelSeparator : '',
            labelWidth     : 0,
            items: [{
              boxLabel       : '使用する',
              inputValue     : 1,
              name           : 'net_con',
              id             : 'use_con'
            },{
              boxLabel       : '使用しない',
              inputValue     : 0,
              name           : 'net_con',
              id             : 'duse_con'
            }]
          }]
        },{
          labelWidth : 0,
          xtype      :'fieldset',
          title      : '回線負荷監視設定',
          autoHeight : true,
          items: [{
            xtype          : 'radiogroup',
            fieldLabel     : '',
            labelSeparator : '',
            labelWidth     : 0,
            items: [{
              boxLabel       : '使用する',
              inputValue     : 1,
              name           : 'net_mon',
              id             : 'use_mon'
            },{
              boxLabel       : '使用しない',
              inputValue     : 0,
              name           : 'net_mon',
              id             : 'duse_mon'
            }]
          }]
        },{
          xtype       : 'fieldset',
          labelWidth  : 140,
          title       : '回線負荷警告レベル',
          height      : 90,
          defaultType : 'textfield',
          layout      : 'form',
          items: [{
            fieldLabel    : '警告レベル(%)<br>150-999%まで設定可能<br>200%通常の2倍の負荷',
            name          : 'net_alt',
            id            : 'net_alt',
            selectOnFocus : true,
            msgTarget     : 'side',
            maxLength     : 3,
            minLength     : 3,
            allowBlank    : false,
            width         : 30
          }]
        }]
      }]
    });
    form.render(document.body);

    var store = new Ext.data.JsonStore({
      url: '/net',
      fields: ['mon', 'con', 'alt']
    });

    store.on('load', function(s, r, o) {
      if (r[0].get('mon')) {
        Ext.getCmp('use_mon').setValue(true);
      } else {
        Ext.getCmp('duse_mon').setValue(true);
      }
      if (r[0].get('con')) {
        Ext.getCmp('use_con').setValue(true);
      } else {
        Ext.getCmp('duse_con').setValue(true);
      }
      var alt = r[0].get('alt');
      if (alt !== '') {
        Ext.getCmp('net_alt').setValue(alt);
      } else {
        Ext.getCmp('net_alt').setValue('300');
      }
    });
    store.load();
    form.render(document.body);

    Ext.getCmp('use_mon').on('focus',function(f,nv,ov){
      form.updated = true;dlg.buttons[2].enable();
    });
    Ext.getCmp('duse_mon').on('focus',function(f,nv,ov){
      form.updated = true;dlg.buttons[2].enable();
    });
    Ext.getCmp('use_con').on('focus',function(f,nv,ov){
      form.updated = true;dlg.buttons[2].enable();
    });
    Ext.getCmp('duse_con').on('focus',function(f,nv,ov){
      form.updated = true;dlg.buttons[2].enable();
    });
    Ext.getCmp('net_alt').on('change',function(f,nv,ov){
      form.updated = true;dlg.buttons[2].enable();
    });

    dlg.form_net = form;
  }

  return dlg.form_net;
}

function build_sync_form(dlg) {
  if (!dlg.form_sync) {

    Ext.apply(Ext.form.VTypes, {
      isip: function(value) {
        return (value.match(/^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$/));
      },
      isipText: 'IPアドレス形式ではありません。<br>例）192.168.1.1'
    });

    var form = new Ext.FormPanel({
      frame            : true,
      header           : false,
      title            : '同期',
      id               : 'tabs-sync',
      iconCls          : 'tabs-sync',
      width            : 340,
      labelWidth       : 0,
      trackResetOnLoad : true,
      timeout          : 10,
      url              : '/sync',
      method           : 'POST',
      items: [{
        border: false,
        items: [{
          xtype       : 'fieldset',
          labelWidth  : 140,
          title       : '同期サーバ設定',
          height      : 60,
          defaultType : 'textfield',
          layout      : 'form',
          items: [{
            fieldLabel    : '同期サーバ',
            name          : 'sync_server',
            id            : 'sync_server',
            emptyText     : 'サーバアドレス',
            selectOnFocus : true,
            msgTarget     : 'side',
            maxLength     : 15,
            minLength     : 7,
            allowBlank    : true,
            vtype         : 'isip',
            width         : 120
          }]
        }]
      }]
    });

    var store = new Ext.data.JsonStore({
      url: '/sync',
      fields: ['server']
    });

    store.on('load', function(s, r, o) {
      var server = r[0].get('server');
      if (server !== '') {
        Ext.getCmp('sync_server').setValue(server);
      } else {
        Ext.getCmp('sync_server').setValue('');
      }
    });
    store.load();
    form.render(document.body);

    Ext.getCmp('sync_server').on('change',function(f,nv,ov){
      form.updated = true;dlg.buttons[2].enable();
    });

    dlg.form_sync = form;
  }

  return dlg.form_sync;
}

function build_machine_list_dialog() {
  if (!G.amachine_dlg) {
    // process list dialog's column
    var cm = new Ext.grid.ColumnModel([
    new Ext.grid.RowNumberer(),
    {
      id        : 'hostname',
      header    : 'ホスト名',
      dataIndex : 'hostname',
      width     : 120,
      sortable  : true
    },{
      header    : 'IPアドレス',
      dataIndex : 'ipaddr',
      width     : 85,
      align     : 'right',
      sortable  : true
    },{
      header    : 'MACアドレス',
      dataIndex : 'macaddr',
      width     : 90,
      align     : 'right',
      sortable  : true
    },{
      header    : 'CPU',
      dataIndex : 'cpu',
      width     : 200,
      sortable  : true
    },{
      header    : 'グラボ',
      dataIndex : 'grabo',
      width     : 200,
      sortable  : true
    },{
      header    : 'メモリ',
      dataIndex : 'mem',
      width     : 40,
      align     : 'right',
      renderer  : Ext.util.Format.jpMoney = function(v){
        v = v/(1024*1024*1024);
        n = v * 100;
        n = Math.round(n);
        n = n / 100;
        return n + "G";
      },
      sortable  : true
    }]);
    cm.defaultSortable = true;

    var ds = new Ext.data.JsonStore({
      url        : '/machineinfolist',
      storeId    : 'hostname',
      fields     : [
        {name: 'hostname',    type: 'string'},
        {name: 'ipaddr',      type: 'string'},
        {name: 'macaddr',     type: 'string'},
        {name: 'grabo',       type: 'string'},
        {name: 'mem',         type: 'int'   },
        {name: 'cpu',         type: 'string'}
      ]
    });

    // build process list dialog
    var grid = new Ext.grid.GridPanel( {
      region           : 'center',
      border           : false,
      ds               : ds,
      cm               : cm,
      autoExpandColumn : 'hostname',
      viewConfig       : {forceFit: true},
      height           : 340,
      loadMask         : {msg:'マシン構成リストデータ取得中...'},
      autoScroll       : true,
      layout           : 'fit',
      stripeRows       : true,
      switch_target    : function(target) {
        ds.load();
      }
    });

    var dlg_close = function() {
      G.amachine_dlg.hide();
      G.amachine_dlg = null;
    };

    var dlg = new Ext.Window({
      title         : 'マシン構成リスト',
      animateTarget : 'act-btn-mach',
      iconCls       : 'btn-macha',
      closeAction   : 'hide',
      layout        : 'fit',
      border        : false,
      plain         : true,
      closable      : false,
      resizable     : false,
      height        : 400,
      width         : 850,
      modal         : true,
      proxyDrag     : true,
      shadow        : true,
      items         : [grid],
      buttons    : [{
        text    : '閉じる',
        handler : function() {
          dlg_close();
        }
      }],
      keys: [{
        key     : Ext.EventObject.ESC,
        handler : function() {
          dlg_close();
        }
      }]
    });

    // set event handler for grid rows
    grid.on('rowdblclick', function(grid, idx, e) {
      var md_hostname = grid.getStore().getAt(idx).get('hostname');
      var md_ipaddr   = grid.getStore().getAt(idx).get('ipaddr')  ;
      var md_macaddr  = grid.getStore().getAt(idx).get('macaddr') ;
      var md_grabo    = grid.getStore().getAt(idx).get('grabo')   ;
      var md_mem      = grid.getStore().getAt(idx).get('mem')     ;
      var md_cpu      = grid.getStore().getAt(idx).get('cpu')     ;
      var md_mem_g    = Math.round(md_mem/(1024*1024*1024) * 100) / 100;

      var detail_infomation = 'マシン名：'    + md_hostname + '<br>' 
                            + 'IPアドレス：'  + md_ipaddr   + '<br>' 
                            + 'MACアドレス：' + md_macaddr  + '<br>' 
                            + 'グラボ：'      + md_grabo    + '<br>' 
                            + 'ＣＰＵ：'      + md_cpu      + '<br>' 
                            + 'メモリ：'      + md_mem_g + 'GBytes (' +md_mem  + 'Bytes)';

      this.detail_info_dlg = new Ext.Window({
        id          : 'detail_info_dlg',
        title       : '詳細',
        iconCls     : 'btn-macha',
        closeAction : 'close',
        closable    : false,
        shadow      : true,
        draggable   : true,
        resizable   : false,
        fixedcenter : true,
        collapsible : false,
        shim        : true,
        modal       : true,
        width       : 400,
        height      : 150,
        layout      : 'fit',
        buttonAlign :'right',
        constrain   : true,
        items       : [{
            html    : detail_infomation
        }],
        buttons     : [{
          text      : '閉じる',
          handler: function() {
            this.detail_info_dlg.close();
          },
          scope:this
        }]
      });
      this.detail_info_dlg.setTitle('詳細@'+md_hostname);
      this.detail_info_dlg.show();
    });

    grid.switch_target();
    dlg.grid = grid;
    G.amachine_dlg = dlg;
  }
  else
  {
    G.amachine_dlg.hide();
  }
  return G.amachine_dlg;
}

function build_network_dialog(hostname,macaddr) {
  if (!G.net_dlg) {

    var ds = new Ext.data.JsonStore({
      url        : '/connectionlist',
      baseParams : { 'macaddr': macaddr },
      storeId    : 'remote_addr',
      sortInfo   : {field:'remote_port', direction:'ASC'},
      fields     : [
        {name: 'local_addr',   type: 'string'},
        {name: 'local_port',   type: 'int'   },
        {name: 'remote_addr',  type: 'string'},
        {name: 'remote_port',  type: 'int'   },
        {name: 'status',       type: 'string'}
      ]
    });

    // process list dialog's column
    var cm = new Ext.grid.ColumnModel([
    new Ext.grid.RowNumberer(),
    {
      id: 'col-remote_addr',
      header: '接続先IP',
      dataIndex: 'remote_addr',
      width: 120,
      align: 'right',
      sortable: true
    },{
      header: '接続元ポート',
      dataIndex: 'local_port',
      width: 60,
      align: 'right',
      sortable: true
    },{
      header: '接続先ポート',
      dataIndex: 'remote_port',
      width: 60,
      align: 'right',
      sortable: true
    //},{
    //  header: 'ステータス',
    //  dataIndex: 'status',
    //  width: 100,
    //  align: 'right',
    //  sortable: true
    }]);
    cm.defaultSortable = true;

    // build process list dialog
    var grid = new Ext.grid.GridPanel( {
      region           : 'center',
      border           : false,
      ds               : ds,
      cm               : cm,
      autoExpandColumn : 'pid',
      viewConfig       : {forceFit: true},
      height           : 340,
      width            : 455,
      loadMask         : {msg:'接続先データ取得中...'},
      autoScroll       : true,
      layout           : 'fit',
      stripeRows       : true,
      switch_target    : function(target) {
        ds.load();
      }
    });

    var dlg_close = function() {
      G.net_dlg.hide();
      G.net_dlg = null;
    };

    var dlg = new Ext.Window({
      animateTarget : 'act-btn-net',
      iconCls       : 'btn-proc',
      layout        : 'fit',
      border        : false,
      plain         : true,
      closeAction   : 'hide',
      closable      : false,
      resizable     : false,
      height        : 410,
      width         : 450,
      modal         : true,
      proxyDrag     : true,
      shadow        : true,
      shadowOffset  : 10,
      constrain     : true,
      items         : [grid],
      tbar          : [{
        cls     : 'x-btn-text-icon',
        text    : '更新',
        icon    : ICON_PATH+'/ah.png',
        tooltip : '接続一覧を更新します',
        handler : function() {
          ds.load();
        },
        scope: this
      }],
      buttons   : [{
        text    : '閉じる',
        handler : function() {
          dlg_close();
        }
      }],
      keys: [{
        key     : Ext.EventObject.ESC,
        handler : function() {
          dlg_close();
        }
      }]
    });

    grid.switch_target();
    dlg.grid = grid;
    G.net_dlg = dlg;
  }
  return G.net_dlg;
}

function build_machine_dialog(hostname, macaddr) {
  if (!G.machine_dlg) {
    // process list dialog's column
    var cm = new Ext.grid.ColumnModel([{
      id: 'col-name',
      header: '対象',
      dataIndex: 'name',
      width: 220,
      align: 'right',
      sortable: true
    },{
      header: '値',
      dataIndex: 'val',
      width: 260,
      align: 'left',
      sortable: true
    }]);
    cm.defaultSortable = true;

    // process list dialog's column
    var ds = new Ext.data.JsonStore({
      url        : '/machineinfo',
      baseParams : { 'macaddr': macaddr },
      storeId    : 'name',
      fields     : [
        {name: 'name',     type: 'string'},
        {name: 'val',      type: 'string'}
      ]
    });
    //ds.load();

    // build process list dialog
    var grid = new Ext.grid.GridPanel( {
      region           : 'center',
      border           : false,
      ds               : ds,
      cm               : cm,
      autoExpandColumn : 'hostname',
      viewConfig       : {forceFit: true},
      height           : 340,
      loadMask         : {msg:'マシン構成リストデータ取得中...'},
      autoScroll       : true,
      layout           : 'fit',
      stripeRows       : true,
      switch_target    : function(target) {
        ds.load();
      }
    });

    var dlg_close = function() {
      G.machine_dlg.hide();
      G.machine_dlg = null;
    };

    var dlg = new Ext.Window({
      animateTarget : 'act-btn-mach',
      closeAction   : 'hide',
      layout        : 'fit',
      border        : false,
      plain         : true,
      closable      : false,
      resizable     : false,
      height        : 410,
      width         : 450,
      modal         : true,
      proxyDrag     : true,
      shadow        : true,
      items         : [grid],
      buttons    : [{
        text    : '閉じる',
        handler : function() {
          dlg_close();
        }
      }],
      keys: [{
        key     : Ext.EventObject.ESC,
        handler : function() {
          dlg_close();
        }
      }]
    });

    grid.switch_target();
    dlg.grid = grid;
    G.machine_dlg = dlg;
  }
  return G.machine_dlg;
}

function build_install_dialog(hostname, macaddr) {
  if (!G.install_dlg) {
    // process list dialog's column
    var cm = new Ext.grid.ColumnModel([{
      id: 'col-val',
      header: 'インストール',
      dataIndex: 'val',
      width: 220,
      align: 'right',
      sortable: true
    },{
      header: 'Version',
      dataIndex: 'ver',
      width: 60,
      align: 'right',
      sortable: true,
      renderer  : function(x) {
        switch(x) {
        case '':
          return '不明';
        default:
          return x;
        }
      }
    }]);

    // process list dialog's column
    var ds = new Ext.data.JsonStore({
      url        : '/install',
      baseParams : { 'macaddr': macaddr },
      storeId    : 'val',
      fields     : [
        {name: 'val', type: 'string'},
        {name: 'ver', type: 'string'}
      ]
    });
    //ds.load();

    // build process list dialog
    var grid = new Ext.grid.GridPanel( {
      region           : 'center',
      border           : false,
      ds               : ds,
      cm               : cm,
      autoExpandColumn : 'hostname',
      viewConfig       : {forceFit: true},
      height           : 340,
      loadMask         : {msg:'インストールリストデータ取得中...'},
      autoScroll       : true,
      layout           : 'fit',
      stripeRows       : true,
      switch_target    : function(target) {
        ds.load();
      }
    });

    // set event handler for grid rows
    grid.on('rowdblclick', function(grid, idx, e) {
      window.open('http://www.plustar.jp/proc.php?q=' + grid.getStore().getAt(idx).get('val'),'google_search');
    });

    var dlg_close = function() {
      G.install_dlg.hide();
      G.install_dlg = null;
    };

    var dlg = new Ext.Window({
      animateTarget : 'act-btn-inst',
      closeAction   : 'hide',
      layout        : 'fit',
      border        : false,
      plain         : true,
      closable      : false,
      resizable     : false,
      height        : 410,
      width         : 450,
      modal         : true,
      proxyDrag     : true,
      shadow        : true,
      items         : [grid],
      tbar          : [{
        cls     : 'x-btn-text-icon',
        text    : '更新',
        icon    : ICON_PATH+'/ah.png',
        tooltip : 'インストールモジュール一覧を更新します',
        handler : function() {
          ds.load();
        },
        scope: this
      },'-',{
        cls: 'x-btn-text-icon',
        text: 'モジュール検索',
        icon: ICON_PATH+'/zm.png',
        tooltip: 'インストールモジュールの検索を行います',
        handler: onInstallModSearch,
        scope: this
      }],
      buttons    : [{
        text    : '閉じる',
        handler : function() {
          dlg_close();
        }
      }],
      keys: [{
        key     : Ext.EventObject.ESC,
        handler : function() {
          dlg_close();
        }
      }]
    });

    grid.switch_target();
    dlg.grid = grid;
    G.install_dlg = dlg;
  }
  return G.install_dlg;
}

function onInstallModSearch()
{
  var selected = G.install_dlg.grid.getSelectionModel().getSelected();
  if (!selected) {
    Ext.MessageBox.show({
       title   : 'メッセージ',
       msg     : '検索するインストールモジュールを選択してください。',
       width   : 300,
       buttons : Ext.MessageBox.OK,
       icon    : Ext.MessageBox.INFO
    });
    return;
  }
  var installname = selected.get('val');
  window.open('http://www.plustar.jp/proc.php?q=' + installname, 'google_search');
}

function build_url_dialog(hostname, macaddr) {
  if (!G.url_dlg) {
    // process list dialog's column
    var cm = new Ext.grid.ColumnModel([{
      id: 'col-title',
      header: 'タイトル',
      dataIndex: 'title',
      width: 220,
      align: 'right',
      sortable: true
    }]);

    // process list dialog's column
    var ds = new Ext.data.JsonStore({
      url        : '/url',
      baseParams : { 'macaddr': macaddr },
      storeId    : 'title',
      fields     : [
        {name: 'url',   type: 'string'},
        {name: 'title', type: 'string'}
      ]
    });
    //ds.load();

    var grid = new Ext.grid.GridPanel( {
      region           : 'center',
      border           : false,
      ds               : ds,
      cm               : cm,
      autoExpandColumn : 'hostname',
      viewConfig       : {forceFit: true},
      height           : 340,
      loadMask         : {msg:'ウェブ参照履歴データ取得中...'},
      autoScroll       : true,
      layout           : 'fit',
      stripeRows       : true,
      switch_target    : function(target) {
        ds.load();
      }
    });

    var dlg_close = function() {
      G.url_dlg.hide();
      G.url_dlg = null;
    };

    var dlg = new Ext.Window({
      animateTarget : 'act-btn-url',
      closeAction   : 'hide',
      layout        : 'fit',
      border        : false,
      plain         : true,
      closable      : false,
      resizable     : false,
      height        : 410,
      width         : 450,
      modal         : true,
      proxyDrag     : true,
      shadow        : true,
      items         : [grid],
      buttons    : [{
        text    : '閉じる',
        handler : function() {
          dlg_close();
        }
      }],
      keys: [{
        key     : Ext.EventObject.ESC,
        handler : function() {
          dlg_close();
        }
      }]
    });

    grid.switch_target();
    dlg.grid = grid;
    G.url_dlg = dlg;
  }
  return G.url_dlg;
}

function build_wakeup_dialog()
{
  if (!G.wakeup_dlg) {
    // process list dialog's column
    var ds = new Ext.data.JsonStore({
      url        : '/stopmcheinelist',
      storeId    : 'hn',
      sortInfo   : {field:'hn', direction:'ASC'},
      fields     : [
        {name: 'hn', type: 'string'},
        {name: 'mc', type: 'string'}
      ]
    });
    ds.load();

    var cmb = new Ext.form.ComboBox({
        hiddenName: "macaddr",  // ← これが正解
        //name: "macaddr",  // ←これは駄目
        fieldLabel: '起動マシン',
        allowBlank: false,
        store: ds,
        displayField: 'hn',
        valueField: 'mc',
        editable: false,
        typeAhead: true,
        mode: 'local',  // ローカルデータを使用
        forceSelection: true,
        triggerAction: 'all',
        emptyText: '対象マシンを選択してください',
        selectOnFocus: true
    });

    // フォームパネルの生成   
    var grid = new Ext.form.FormPanel({
        url               : '/wakeup',
        method            : 'POST',
        trackResetOnLoad  : true,
        timeout           : 10,
        standardSubmit    : true,  // 通常のサブミット処理を行う
        labelWidth        : 70,
        frame             : true,
        bodyStyle         : 'padding:5px 5px 0',
        width             : 200,
        defaultType       : 'textfield',
        //items: formItems,
        items: [cmb],
        buttons    : [{
        text    : '起動',
        handler : function() {
          var mac = cmb.getValue();
          if (mac.length === 0) {
            Ext.MessageBox.show({
               title   : 'エラー',
               msg     : '対象マシンを選択してください',
               width   : 300,
               buttons : Ext.MessageBox.OK,
               icon    : Ext.MessageBox.ERROR
            });
            cmb.focus();
            return;
          } else {
            Ext.Ajax.request({
              url: '/wakeup',
              params: { 'macaddr': mac }
            });
            Ext.MessageBox.show({
               title   : 'メッセージ',
               msg     : '起動処理要求を送信しました<br>暫くすると起動します<br><br><a href="'+HELPPAGE_URL+'8" target="_blank">起動しない場合...</a>',
               width   : 300,
               buttons : Ext.MessageBox.OK,
               icon    : Ext.MessageBox.INFO
            });
            dlg_close();
          }
        }
      },{
        text    : '閉じる',
        handler : function() {
          dlg_close();
        }
      }]
    });

    var dlg_close = function() {
      dlg.hide();
      G.wakeup_dlg = null;
    };

    var dlg = new Ext.Window({
      animateTarget : 'act-btn-wakeup',
      closeAction   : 'hide',
      layout        : 'fit',
      border        : false,
      plain         : true,
      closable      : false,
      resizable     : false,
      height        : 123,
      width         : 400,
      modal         : true,
      proxyDrag     : true,
      shadow        : true,
      items         : [grid],
      keys: [{
        key     : Ext.EventObject.ESC,
        handler : function() {
          dlg_close();
        }
      }]
    });

    G.wakeup_dlg = dlg;
  }
  return G.wakeup_dlg;
}

function build_updatehistory_dialog()
{
  if (!G.updatehistory_dlg) {
    var cm = new Ext.grid.ColumnModel([{
      id: 'col-dy',
      header: 'リリース日',
      dataIndex: 'dy',
      width: 120,
      align: 'right',
      sortable: true
    },{
      header: '更新内容',
      dataIndex: 'fu',
      width: 260,
      align: 'left',
      sortable: true
    }]);

    var ds = new Ext.data.JsonStore({
      url        : '/updatehistory',
      storeId    : 'dy',
      sortInfo   : {field:'dy', direction:'DESC'}, // リリース日でソート
      fields     : [
        {name: 'dy', type: 'string'}, // リリース年月日
        {name: 'fu', type: 'string'}  // 追加機能
      ]
    });

    var grid = new Ext.grid.GridPanel( {
      region           : 'center',
      border           : false,
      ds               : ds,
      cm               : cm,
      autoExpandColumn : 'dy',
      viewConfig       : {forceFit: true},
      height           : 340,
      loadMask         : {msg:'アップデート履歴データ取得中...'},
      autoScroll       : true,
      layout           : 'fit',
      stripeRows       : true,
      switch_target    : function(target) {
        ds.load();
      }
    });

    var dlg_close = function() {
      G.updatehistory_dlg.hide();
      G.updatehistory_dlg = null;
    };

    var dlg = new Ext.Window({
      animateTarget : 'act-btn-help',
      iconCls       : 'btn-his',
      closeAction   : 'hide',
      layout        : 'fit',
      border        : false,
      plain         : true,
      closable      : false,
      resizable     : false,
      height        : 410,
      width         : 450,
      modal         : true,
      proxyDrag     : true,
      shadow        : true,
      items         : [grid],
      buttons    : [{
        text    : '閉じる',
        handler : function() {
          dlg_close();
        }
      }],
      keys: [{
        key     : Ext.EventObject.ESC,
        handler : function() {
          dlg_close();
        }
      }]
    });

    grid.switch_target();
    dlg.grid = grid;
    G.updatehistory_dlg = dlg;
  }
  return G.updatehistory_dlg;
}

function build_wait_dialog(msg)
{
  Ext.MessageBox.show({
    title        : 'しばらくお待ちください...',
    msg          : msg,
    progressText : '取得中...',
    width        : 300,
    progress     : true,
    closable     : false
  });

  // this hideous block creates the bogus progress
  var f = function(v){
    return function(){
      if(v == 12){
        Ext.MessageBox.hide();
      }else{
        var i = v/11;
        Ext.MessageBox.updateProgress(i, Math.round(100*i)+'% 完了');
      }
    };
  };
  for(var i = 1; i < 13; i++){
    setTimeout(f(i), i*500);
  }
}
// ----------------------------------------------------------------------
// action
var action = {
  perform: function(act, fn, cfm) {
    var selected = G.main_wnd.grid.getSelectionModel().getSelected();
    if (!selected) {
      Ext.MessageBox.show({
         title   : '注意',
         msg     : act + 'するPCを選択してください。<br><a href="'+HELPPAGE_URL+'5" target="_blank">詳細情報...</a>',
         width   : 300,
         buttons : Ext.MessageBox.OK,
         icon    : Ext.MessageBox.WARNING
      });
      return;
    }

    var macaddr = selected.id;
    var hostname = selected.data.hostname;
    if (cfm) {
      Ext.MessageBox.confirm(
        '確認',
        String.format(
          'マシン名 "{0}" のPCを{1}しようとしています。<br />よろしいですか?',
          hostname, act),
        function(btn){if (btn=='yes') fn(hostname, macaddr);});
    } else {
      fn(hostname, macaddr);
    }
  },
  wakeup: function() {
    var dlg = build_wakeup_dialog();
    dlg.setTitle('マシンの起動');
//    dlg.center();
    dlg.show();
  },
  shutdown: function() {
    action.perform('シャットダウン', function(hostname, macaddr) {
      Ext.Ajax.request({
        url: '/shutdown',
        params: { 'macaddr': macaddr }
      });
    }, true);
  },
  reboot: function() {
    action.perform('再起動', function(hostname, macaddr) {
      Ext.Ajax.request({
        url: '/reboot',
        params: { 'macaddr': macaddr }
      });
    }, true);
  },
  process: function() {
    action.perform('プロセス一覧を取得', function(hostname, macaddr) {
      var dlg = build_process_dialog(hostname, macaddr);
      if(dlg !== null)
      {
        dlg.setTitle('プロセス一覧@'+hostname);
        dlg.show();
      }
      else
      {
        var msg = 'プロセス情報取得中...';
        build_wait_dialog(msg);
      }
    });
  },
  psync: function() {
    action.perform('同期を実行する対象', function(hostname, macaddr) {
      Ext.MessageBox.show({
         title   : 'エラー',
         iconCls : 'btn-sync',
         width   : 300,
         msg     : '同期システムは未契約です。<br><a href="'+HELPPAGE_URL+'98" target="_blank">詳細情報...</a>',
         buttons : Ext.MessageBox.OK,
         icon    : Ext.MessageBox.ERROR
      });
    });
  },
  amachinelist: function() {
    var dlg = build_machine_list_dialog();
    dlg.setTitle('マシン構成');
    dlg.show();
  },
  option: function() {
    var dlg = build_option_dialog();
//    dlg.form_pwd.reset();
    dlg.suppression_grid.switch_target();
    dlg.buttons[2].disable();
//    dlg.getTabs().activate(0);
    dlg.show();
  },
  manual: function() {
    window.open(HELPPAGE_URL+'help');
  },
  log: function() {
    var dlg = build_log_dialog();
    dlg.setTitle('クライアント通知ログ');
    dlg.show();
  },
  license: function() {
    var dlg = build_license_dialog();
    dlg.show();
  },
  analyzer: function() {
    window.open(HELPPAGE_URL+'ext');
  },
  net: function() {
    action.perform('接続先を取得する', function(hostname, macaddr) {
      var dlg = build_network_dialog(hostname, macaddr);
      if(dlg !== null)
      {
        dlg.setTitle('接続先一覧@'+hostname);
        dlg.show();
      }
      else
      {
        var msg = '接続先情報取得中...';
        build_wait_dialog(msg);
      }
    });
  },
  machinelist: function() {
    action.perform('マシン構成を取得する', function(hostname, macaddr) {
      var dlg = build_machine_dialog(hostname, macaddr);
      dlg.setTitle('マシン構成@'+hostname);
      dlg.show();
    });
  },
  install: function() {
    action.perform('インストールアプリケーション情報を取得する', function(hostname, macaddr) {
      var dlg = build_install_dialog(hostname, macaddr);
      dlg.setTitle('インストールアプリケーション情報@'+hostname);
      dlg.show();
    });
  },
  weburl: function() {
    action.perform('ウェブ参照履歴を取得する', function(hostname, macaddr) {
      var dlg = build_url_dialog(hostname, macaddr);
      dlg.setTitle('ウェブ参照履歴情報@'+hostname);
      dlg.show();
    });
  },
  transcope: function() {
    action.perform('メンテナンス履歴を取得する', function(hostname, macaddr) {
      var dlg = build_transcope_dialog(hostname, macaddr);
      dlg.setTitle('メンテナンス履歴情報@'+hostname);
      dlg.show();
    });
  },
  psbug: function() {
    var c_name  = Ext.get('lic_cname').dom.innerHTML;
    var sev_ver = Ext.get('lic_version').dom.innerHTML;
    window.open(CONTACT_URL+'b&c='+c_name+'&s=&ver='+sev_ver,'バグ報告','status=yes,width=700,height=450');
  },
  psreq: function() {
    var c_name  = Ext.get('lic_cname').dom.innerHTML;
    var sev_ver = Ext.get('lic_version').dom.innerHTML;
    window.open(CONTACT_URL+'r&c='+c_name+'&s=&ver='+sev_ver,'機能要望','status=yes,width=700,height=450');
  },
  psqus: function() {
    var c_name  = Ext.get('lic_cname').dom.innerHTML;
    var sev_ver = Ext.get('lic_version').dom.innerHTML;
    window.open(CONTACT_URL+'q&c='+c_name+'&s=&ver='+sev_ver,'質問','status=yes,width=700,height=450');
  },
  updatehistory: function() {
    var dlg = build_updatehistory_dialog();
    dlg.setTitle('アップデート履歴');
    dlg.show();
  },
  screenshot: function()
  {
    // TODO wanna do refactoring
    action.perform('スクリーンショットを取得', function(hostname, macaddr) {
      var prompt = build_screenshot_password_prompt();
      // インスタンス外でないとhostnameを持ちまわり初期化されない
      var title = hostname + 'のスクリーンショット';
      Ext.get('sshot_pwd').focus();
      prompt.callback = function(password) {
        var wait = Ext.MessageBox.show({
            title        : 'しばらくお待ちください...',
            msg          : title + 'を取得しています...',
            progressText : '取得中...',
            width        : 300,
            progress     : true,
            closable     : false
        });

        Ext.Ajax.request({
          url: '/screenshot',
          params: { 'macaddr': macaddr, 'password': password },
          success: function(response, opts) {

            var dlg   = build_screenshot_dialog(title);

            // オブジェクトの実体がないとDOMにアクセスできない
            // 何か格好悪いけど、、、
            //dlg.setSize(0,0);
            dlg.show();
            dlg.hide();

            dlg.body.dom.innerHTML = '<img id="sshot-img" />';
            var img = Ext.get('sshot-img');

            img.set({
              src: '/screenshot?_dc='+new Date().getTime(),
              alt: title
            }).on({
              'click' : {
                fn: handler.sshot_zoom,
                scope: img
              },
              'load' : {
                fn: handler.sshot_zoom,
                scope: img
              }
            });

            // this hideous block creates the bogus progress
            var timer_id = null;
            var f = function(v){
              return function(){
                // 通常ここへはマシンが落ちてない限り来ない
                if(v == 120){
                  if (wait.isVisible()) {
                    wait.hide();
                    dlg.hide();
                    dlg = null;
                    Ext.MessageBox.show({
                       title   : 'エラー',
                       iconCls : 'btn-sshot',
                       msg     : 'スクリーンショットの取得に失敗しました。<br />電源が入っていないか、ログインしていない可能性があります。<br><a href="'+HELPPAGE_URL+'3" target="_blank">詳細情報...</a>',
                       width   : 300,
                       buttons : Ext.MessageBox.OK,
                       icon    : Ext.MessageBox.ERROR
                    });
                  }
                }else{
                  var i = v/119;
                  if(img.actualSize){
                    clearTimeout(timer_id);
                    timer_id = null;
                    if(wait !== null){
                      wait.hide();
                    }
                    wait = null;
                    //dlg.setSize(800,600);
                    dlg.setTitle(title);
                    dlg.show();
                  }
                  else
                  {
                    if(wait !== null){
                      wait.updateProgress(i, Math.round(100*i)+'% 処理中...');
                    }
                  }
                }
              };
            };
            for(var i = 1; i < 121; i++){
              timer_id = setTimeout(f(i), i*500);
            }

            //dlg.setTitle(title);
          },
          failure: function() {
            Ext.MessageBox.show({
               title   : 'エラー',
               iconCls : 'btn-sshot',
               msg     : 'パスワードが不正です。',
               width   : 300,
               buttons : Ext.MessageBox.OK,
               icon    : Ext.MessageBox.ERROR
            });
          }
        });
      };

      prompt.show();
      setTimeout("Ext.get('sshot_pwd').focus()", 1);
    });
  }
};

// ----------------------------------------------------------------------
// handler
var handler = {
  sshot_zoom: function() {
    var dlg = G.sshot_dlg;
    if (!this.actualSize) {
      this.actualSize = { width: this.getWidth(), height: this.getHeight() };
    }
    if (this.getWidth() == this.actualSize.width) {
      dlg.body.setStyle('overflow', 'hidden');
      this.set(dlg.body.getViewSize());
      this.set({title: 'クリックで拡大'});
    } else {
      dlg.body.setStyle('overflow', 'auto');
      this.set(this.actualSize);
      this.set({title: 'クリックで縮小'});
    }
    if (!dlg.isVisible()) {
      dlg.show();
    }
  },
  item_check: function(item, checked) {
    if (checked) {
      if (item.value) {
        thread.restart(item.value);
      } else {
        thread.stop();
      }
    }
  },
  load_main_grid: function(ds) {
    var t = ds.getTotalCount();
    var c = ds.getCount();

    Ext.getDom('boot_pc_count').innerHTML   = c;
    Ext.getDom('boot_pc_total').innerHTML   = t;
    Ext.getDom('boot_pc_ratio').innerHTML   = Math.round(t>0?c*100/t:0);
    Ext.getDom('proc_count').innerHTML      = G.main_wnd.proc_count;
    Ext.getDom('conn_count').innerHTML      = G.main_wnd.conn_count;
    G.main_wnd.proc_count      = 0; // clear process count
    G.main_wnd.conn_count      = 0; // clear connections count
  },
  open_window: function(e, target) {
    window.open(target.href);
  }
};

function build_screenshot_dialog(title) {
  if (!G.sshot_dlg) {
    // build screenshot dialog

    var dlg = new Ext.Window({
      id            : 'sshot-dlg',
      animateTarget : 'act-btn-sshot',
      iconCls       : 'btn-sshot',
      autoScroll    : true,
      autoCreate    : true,
      closeAction   : 'hide',
      closable      : false,
      resizable     : false,
      height        : 600,
      width         : 800,
      modal         : true,
      proxyDrag     : true,
      shadow        : true,
      shadowOffset  : 10,
      buttons       : [{
        text        : '閉じる',
        handler     : function() {
          dlg_close();
        }
      }],
      keys          : [{
        key: Ext.EventObject.ESC,
        handler     : function() {
          dlg_close();
        }
      }]
    });

    var dlg_close = function() {
      Ext.get('sshot-img').actualSize = null;
      dlg.hide();
    };

    G.sshot_dlg = dlg;
  }
  return G.sshot_dlg;
}

// ----------------------------------------------------------------------
// filter
var filter = {
  //img_tag: '<img src="lib/images/{0}.png" alt="{1}" title="{1}" />',
  img_tag: '<div title="{1}" class="sri-{0}" /></div>',
  proc: function(x) {
    G.main_wnd.proc_count += x;
    return x;
  },
  conn: function(x) {
    G.main_wnd.conn_count += x;
    return x;
  },
  meter: function(x) {
    return String.format(filter.img_tag, Math.round(x/10), x+'%');
  }
};

// ----------------------------------------------------------------------
// main thread
var thread = {
  proc: function() {
    G.main_wnd.grid.getStore().load();
  },
  start: function(interval) {
    if (!this.timer_id) {
      this.proc();
      this.timer_id = setInterval(this.proc, interval*1000);
    }
  },
  stop: function() {
    if (this.timer_id) {
      clearInterval(this.timer_id);
      this.timer_id = null;
    }
  },
  restart: function(interval) {
    this.stop();
    this.start(interval);
  }
};

// ----------------------------------------------------------------------
// init page
Ext.onReady(function() {
  var fbc = Ext.getDom('_firebugConsole');
  if(fbc !== null)
  {
    fbc.innerHTML = 'FireBugを無効にしてからお使いください。';
    Ext.MessageBox.show({
       title   : 'メッセージ',
       msg     : 'FireBugを無効にしてからお使いください。<br><a href="'+HELPPAGE_URL+'7" target="_blank">エラー詳細情報...</a>',
       width   : 300,
       buttons : Ext.MessageBox.OK,
       icon    : Ext.MessageBox.WARNING
    });
    return;
  }

  function add_open_window_listener(target) {
    Ext.get(target).on({
      click: {
        stopEvent: true,
        delegate: 'a',
        fn: function(e, target) { window.open(target.href); }
      }
    });
  }
  add_open_window_listener('powered');
  add_open_window_listener('copyright');

  var viewport = new Ext.Viewport({
    layout: 'border',
    window:{
      layout:'fit'
    },
    items: [{
      // ------------------------
      // Header panel
      region : 'north',
      el     : 'header',
      xtype  : 'box',
      height : 55,
      border : false
    },{
      // ------------------------
      // Content panel
      region    : 'center',
      layout    : 'fit',
      id        : 'content',
      xtype     : 'box',
      margins   : '0 1 0 0',
      listeners : {
        'resize': function (cmp, aw, ah, rw, rh) {
          if (grid) grid.setSize(aw, ah);
        }
      }
    },{
      // ------------------------
      // Footer panel
      region : 'south',
      el     : 'footer',
      xtype  : 'box',
      height : 16,
      border : false
    }]
  });

  // main grid's data source
  var ds = new Ext.data.JsonStore({
    url           : '/machinelist',
    root          : 'results',
    totalProperty : 'total',
    storeId       : 'hostname',
    id            : 'macaddr',
    fields        : [
      {name: 'user_job',    type: 'int'   },
      {name: 'hostname',    type: 'string'},
      {name: 'ipaddr',      type: 'string'},
      {name: 'boot_time',   type: 'date'  },
      {name: 'time_span',   type: 'string'},
      {name: 'proc',        type: 'int'   },
      {name: 'connections', type: 'int'   },
      {name: 'mem',         type: 'int'   },
      {name: 'disk',        type: 'int'   },
      {name: 'cpu',         type: 'int'   },
      {name: 'traffic',     type: 'int'   },
      {name: 'net_in',      type: 'int'   },
      {name: 'net_out',     type: 'int'   }
    ]
  });
  ds.on('load', handler.load_main_grid);
  ds.on('load', function(s, r, o) {
    var c = s.getCount();
    if (c < 1) {
      var msg = 'ネットワーク構成情報取得中...';
      build_wait_dialog(msg);
    }
  });

  // main grid's column
  var cm = new Ext.grid.ColumnModel([
  new Ext.grid.RowNumberer(),
  {
    header    : '利用',
    dataIndex : 'user_job',
    width     : 41,
    tooltip   : '各端末の利用状況<br>'
              + '&nbsp;<img src='+ICON_PATH+'/tr.png />：管理端末<br>'
              + '&nbsp;<img src='+ICON_PATH+'/se.png />：端末使用中<br>'
              + '&nbsp;<img src='+ICON_PATH+'/sg.png />：オンラインゲーム<br>'
              + '&nbsp;<img src='+ICON_PATH+'/ss.png />：ストリーミング<br>'
              + '&nbsp;<img src='+ICON_PATH+'/stv.png />：動画再生<br>'
              + '&nbsp;<img src='+ICON_PATH+'/sd.png />：ダウンロード<br>'
              + '&nbsp;<img src='+ICON_PATH+'/su.png />：アップロード<br>'
              + '&nbsp;<img src='+ICON_PATH+'/sw.png />：ネットサーフィン<br>'
              + '&nbsp;<img src='+ICON_PATH+'/son.png />：起動<br>',
    sortable  : true,
    align     : 'center',
    renderer  : function(x) {
      switch(x) {
      // Case0：起動のみ
      case 0:
        return '<img src="'+ICON_PATH+'/son.png" alt="起動" title="起動" />';
      // Case1：管理サーバ
      case 1:
        return '<img src="'+ICON_PATH+'/tr.png" alt="管理" title="管理" />';
      // Case2：ストリーミング
      case 2:
        return '<img src="'+ICON_PATH+'/ss.png" alt="ストリーミング" title="ストリーミング" />';
      //  Case3：動画
      case 3:
        return '<img src="'+ICON_PATH+'/stv.png" alt="動画" title="動画" />';
      // Case4：ダウンロード,P2P
      case 4:
        return '<img src="'+ICON_PATH+'/sd.png" alt="ダウンロード" title="ダウンロード" />';
      // Case5：データ送信
      case 5:
        return '<img src="'+ICON_PATH+'/su.png" alt="アップロード" title="アップロード" />';
      // Case6：オンラインゲームゲーム
      case 6:
        return '<img src="'+ICON_PATH+'/sg.png" alt="ゲーム" title="ゲーム" />';
      // Case7：ウェブサーフィン
      case 7:
        return '<img src="'+ICON_PATH+'/sw.png" alt="WEB" title="WEB" />';
      // Case8：端末使用中
      case 8:
        return '<img src="'+ICON_PATH+'/se.png" alt="使用中" title="使用中" />';
      default:
        return '<img src="'+ICON_PATH+'/son.png" alt="起動" title="起動" />';
      }
    }
  },{
    id        : 'col-hostname',
    header    : 'マシン名',
    dataIndex : 'hostname',
    width     : 140,
    tooltip   : 'マシン名',
    sortable  : true
  },{
    header    : 'IP',
    dataIndex : 'ipaddr',
    width     : 90,
    tooltip   : 'IPアドレス',
    sortable  : true
  },{
    header    : '起動時間',
    dataIndex : 'boot_time',
    width     : 110,
    tooltip   : '起動した時間',
    renderer  : Ext.util.Format.dateRenderer('Y/m/d H:i:s'),
    sortable  : true
  },{
    header    : '経過時間',
    dataIndex : 'time_span',
    width     : 60,
    tooltip   : '経過した時間',
    align     : 'right',
    sortable  : true
  },{
    header    : 'プロセス',
    dataIndex : 'proc',
    width     : 60,
    tooltip   : '起動プロセス数',
    align     : 'right',
    renderer  : filter.proc,
    sortable  : true
  },{
    header    : '接続数',
    dataIndex : 'connections',
    width     : 60,
    tooltip   : '外部接続数',
    align     : 'right',
    renderer  : filter.conn,
    sortable  : true
  },{
    header    : 'メモリ',
    dataIndex : 'mem',
    width     : 68,
    tooltip   : 'メモリ使用率',
    align     : 'center',
    renderer  : filter.meter,
    sortable  : true
  },{
    header    : 'ディスク',
    dataIndex : 'disk',
    width     : 68,
    tooltip   : 'ディスク使用率',
    align     : 'center',
    renderer  : filter.meter,
    sortable  : true
  },{
    header    : 'CPU',
    dataIndex : 'cpu',
    width     : 68,
    tooltip   : 'CPU使用率',
    align     : 'center',
    renderer  : filter.meter,
    sortable  : true
  },{
    header    : '通信量',
    dataIndex : 'traffic',
    width     : 68,
    tooltip   : '全体通信量に対する端末の使用量(%)',
    align     : 'center',
    renderer  : filter.meter,
    sortable  : true
  },{
    header    : '受信',
    dataIndex : 'net_in',
    width     : 68,
    tooltip   : '対象端末の受信量(%)',
    align     : 'center',
    renderer  : filter.meter,
    sortable  : true
  },{
    header    : '送信',
    dataIndex : 'net_out',
    tooltip   : '対象端末の送信量(%)',
    width     : 68,
    align     : 'center',
    renderer  : filter.meter,
    sortable  : true
  }]);
  cm.defaultSortable = false;

  var grid = new Ext.grid.GridPanel( {
    renderTo         : 'grid',
    border           : true,
    ds               : ds,
    cm               : cm,
    sm               : new Ext.grid.RowSelectionModel({singleSelect:true}),
    autoExpandColumn : 'col-hostname',
    viewConfig       : {forceFit: true},
    autoWidth        : true,
    stripeRows       : true,
    autoScroll       : true,
    tbar: [{
      id      : 'act-btn-procs',
      cls     : 'x-btn-text-icon',
      text    : '',
      iconCls : 'btn-proc',
      tooltip : '選択したPCのプロセス一覧を取得します',
      handler : action.process,
      scope   : this
    },{
      id      : 'act-btn-net',
      cls     : 'x-btn-text-icon',
      text    : '',
      iconCls : 'btn-net',
      tooltip : '選択したPCの接続先一覧を表示します',
      handler : action.net,
      scope   : this
    },{
      id      : 'act-btn-mach',
      cls     : 'x-btn-text-icon',
      text    : '',
      iconCls : 'btn-mach',
      tooltip : '選択したPCのマシン構成を表示します',
      handler : action.machinelist,
      scope   : this
    },{
      id      : 'act-btn-inst',
      cls     : 'x-btn-text-icon',
      text    : '',
      iconCls : 'btn-inst',
      tooltip : '選択したPCのインストールリストを表示します',
      handler : action.install,
      scope   : this
    //},{
    //  id      : 'act-btn-transcope',
    //  cls     : 'x-btn-text-icon',
    //  text    : '',
    //  iconCls : 'btn-transcope',
    //  tooltip : 'メンテナンス履歴を表示します',
    //  handler : action.transcope,
    //  scope   : this
    },'-',{
      id      : 'act-btn-sync',
      cls     : 'x-btn-text-icon',
      text    : '',
      iconCls : 'btn-sync',
      tooltip : '選択したPCのマシン間の同期を行います',
      handler : action.psync,
      scope   : this
    },{
      id      : 'act-btn-sshot',
      cls     : 'x-btn-text-icon',
      text    : '',
      iconCls : 'btn-sshot',
      tooltip : '選択したPCのスクリーンショットを取得します',
      handler : action.screenshot,
      scope   : this
    },'-',{
      id      : 'act-btn-wakeup',
      cls     : 'x-btn-text-icon',
      text    : '',
      icon    : ICON_PATH+'/ac.png',
      tooltip : '選択したPCを起動します',
      handler : action.wakeup,
      scope   : this
    },{
      id      : 'act-btn-shotdown',
      cls     : 'x-btn-text-icon',
      text    : '',
      icon    : ICON_PATH+'/ca.png',
      tooltip : '選択したPCをシャットダウンします',
      handler : action.shutdown,
      scope   : this
    },{
      id      : 'act-btn-reboot',
      cls     : 'x-btn-text-icon',
      text    : '',
      icon    : ICON_PATH+'/ar.png',
      tooltip : '選択したPCを再起動します',
      handler : action.reboot,
      scope   : this
    },'-',{
      id      : 'act-btn-analyzer',
      cls     : 'x-btn-text-icon',
      text    : '',
      icon    : ICON_PATH+'/ae.png',
      tooltip : '稼動プロセスアナライザー',
      handler : action.analyzer
    },'-',{
      id      : 'act-btn-tools',
      icon    : ICON_PATH+'/wh.png',
      tooltip : '各種設定およびログの確認ができます',
      cls     : 'x-btn-text-icon',
      text    : '',
      menu    : {items: [{
        icon  : ICON_PATH+'/cl.png',
        cls   : 'x-btn-text-icon',
        text  : '更新間隔',
        menu  :  {items: [{
          id           : 'interval-10',  // テストで挿入する
          text         : '10秒',
          value        : 10,
          checked      : false,
          group        : 'interval',
          checkHandler : handler.item_check
        },{
          id           : 'interval-20',
          text         : '20秒',
          value        : 20,
          checked      : false,
          group        : 'interval',
          checkHandler : handler.item_check
        },{
          id           : 'interval-30',
          text         : '30秒',
          value        : 30,
          checked      : false,
          group        : 'interval',
          checkHandler : handler.item_check
        },{
          id           : 'interval-60',
          text         : '60秒',
          value        : 60,
          checked      : false,
          group        : 'interval',
          checkHandler : handler.item_check
        },{
          id           : 'interval-120',
          text         : '120秒',
          value        : 120,
          checked      : false,
          group        : 'interval',
          checkHandler : handler.item_check
        },{
          id           : 'interval-0',
          text         : '無効',
          value        : 0,
          checked      : false,
          group        : 'interval',
          checkHandler : handler.item_check
        }]}
      },{
        text           : 'クライアント通知ログ...',
        iconCls        : 'btn-log',
        handler        : action.log
      },{
        text           : 'マシン構成リスト...',
        icon           : ICON_PATH+'/ml.png',
        handler        : action.amachinelist
      },{
        text           : 'オプション...',
        iconCls        : 'btn-opt',
        handler        : action.option
      }]}
    },{
      id : 'act-btn-help',
      icon: ICON_PATH+'/hp.png',
      cls: 'x-btn-text-icon',
      text: '',
      menu: {items: [{
        text: 'オンラインマニュアル',
        icon: ICON_PATH+'/bk.png',
        handler: action.manual
      },'-',{
        text: 'バグ報告...',
        iconCls: 'btn-bug',
        handler: action.psbug
      },{
        text: '機能要望...',
        iconCls: 'btn-req',
        handler: action.psreq
      },{
        text: '質問...',
        iconCls: 'btn-qes',
        handler: action.psqus
      },'-',{
        text: 'アップデート履歴',
        iconCls: 'btn-his',
        handler: action.updatehistory
      },'-',{
        text: 'ライセンス',
        iconCls: 'btn-key',
        handler: action.license
      }]}
    }],
    bbar: new Ext.Toolbar({
      items: [
        'PC合計: <span id="boot_pc_total">0</span>台',
        '-',
        '起動PC: <span id="boot_pc_count">0</span>台',
        '-',
        '起動PC率: <span id="boot_pc_ratio">0</span>%',
        '-',
        'プロセス合計: <span id="proc_count">0</span>',
        '-',
        '接続数合計: <span id="conn_count">0</span>',
        '->'
      ]
    })
  });
  grid.setHeight(Ext.getBody().dom.clientHeight-64);

  //grid.on('rowdblclick', function(grid, idx, e) {
  grid.on('rowcontextmenu', function(grid, idx, e) {
    var selected = grid.getSelectionModel().getSelections();
    //if (selected.length > 0) {
    //}
    if(!this.clickharukomenu){
    //var clickharukomenu = new Ext.menu.Menu({
    this.clickharukomenu = new Ext.menu.Menu({
      id: 'harumenu',
      items: [
        {
          text    : 'プロセス',
          iconCls : 'btn-proc',
          handler : action.process,
          scope   : this
        },{
          text    : '接続先',
          iconCls : 'btn-net',
          handler : action.net,
          scope   : this
        },{
          text    : 'マシン構成',
          iconCls : 'btn-mach',
          handler : action.machinelist,
          scope   : this
        },{
          text    : 'インストールリスト',
          iconCls : 'btn-inst',
          handler : action.install,
          scope   : this
        //},{
        //  text    : 'メンテナンス履歴',
        //  iconCls : 'btn-transcope',
        //  handler : action.transcope,
        //  scope   : this
        },'-',{
          cls     : 'x-btn-text-icon',
          text    : '同期',
          iconCls : 'btn-sync',
          handler : action.psync,
          scope   : this
        },{
          text    : 'スクリーンショット',
          iconCls : 'btn-sshot',
          handler : action.screenshot,
          scope   : this
        },'-',{
          text    : 'シャットダウン',
          icon    : ICON_PATH+'/ca.png',
          handler : action.shutdown,
          scope   : this
        },{
          text    : '再起動',
          icon    : ICON_PATH+'/ar.png',
          handler : action.reboot,
          scope   : this
        }
     ]
    });
      //this.clickharukomenu.on('hide', this.onContextHide, this);
    }
    e.stopEvent();
    if(this.ctxRow){
      Ext.fly(this.ctxRow).removeClass('x-node-ctx');
      this.ctxRow = null;
    }
    this.ctxRow = this.view.getRow(idx);
    this.ctxRecord = this.store.getAt(idx);
    Ext.fly(this.ctxRow).addClass('x-node-ctx');
    this.clickharukomenu.showAt(e.getXY());
  });

//  // IE have rendering issue, relayout fixes it
//  if (Ext.isIE) {
//    layout.layout();
//  }

  G.main_wnd.grid = grid;

  // start thread
  Ext.getCmp('interval-'+DEFAULT_INTERVAL).setChecked(true);
});
