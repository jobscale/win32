/*
 * extrapper.js - JavaScript for ExTrapper
 * (c) 2006-2007 Plustar Co. Ltd.
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
// init Ext
//
Ext.BLANK_IMAGE_URL = 'lib/images/s.gif';
Ext.QuickTips.init();

// ----------------------------------------------------------------------
// global variable
//
var G = {
  main_wnd: {
    grid: null,
    proc_count: 0,
    conn_count: 0,
    agefilter_count: 0
  },
  proc_dlg: null,
  sshot_dlg: null,
  opt_dlg: null,
  lic_dlg: null,
  net_dlg: null,
  machine_dlg: null,
  install_dlg: null,
  url_dlg: null,
  stop_log_dlg: null
};

// ----------------------------------------------------------------------
// build functions
//
function build_process_dialog(hostname, macaddr) {
  if (!G.proc_dlg) {
    var ds = new Ext.data.JsonStore({
      url        : '/process',
      baseParams : { 'macaddr': macaddr},
      storeId    : 'pid',
      fields     : [
        {name: 'pid',       type: 'int'   },
        {name: 'sid',       type: 'int'   },
        {name: 'procname',  type: 'string'},
        {name: 'begin',     type: 'date'  },
        {name: 'time_span', type: 'string'}
      ]
    });
    ds.load();

    // build process list dialog
    var dlg = new Ext.LayoutDialog('proc-dlg', {
      animateTarget: 'act-btn-procs',
      autoCreate: true,
      autoScroll: true,
      resizable: false,
      closeAction : "hide",
      closable : false,
      height: 410,
      width: 455,
      modal: true,
      proxyDrag: true,
      shadow: true,
      shadowOffset  : 10,
      center: {
      }
    });

    var dlg_close = function() {
      dlg.hide();
      G.proc_dlg = null;
    };

    // setup buttons
    //dlg.addKeyListener(27, dlg_close, dlg);
    dlg.addButton('閉じる', dlg_close, dlg);

    // build layout for process list dialog
    var layout = dlg.getLayout();
    layout.beginUpdate();
    layout.add('center', new Ext.ContentPanel('proc-dlg-grid', {
      autoCreate: true, fitToFrame: true
    }));
    layout.endUpdate();

    // set event handler on resize
    layout.findPanel('proc-dlg-grid').on('resize', function(p,w,h) {
      dlg.grid.autoSize();
    });

    // process list dialog's column
    var cm = new Ext.grid.ColumnModel([{
      id: 'col-procname',
      header: 'イメージ名',
      dataIndex: 'procname',
      width: 100,
      sortable: true
    },{
      header: 'PID',
      dataIndex: 'pid',
      width: 35,
      align: 'right',
      sortable: true
    },{
      header: 'セッション ID',
      dataIndex: 'sid',
      width: 20,
      align: 'right',
      sortable: true
    },{
      header: '起動時間',
      dataIndex: 'begin',
      width: 110,
      renderer: Ext.util.Format.dateRenderer('Y/m/d H:i:s'),
      sortable: true
    },{
      header: '経過時間',
      dataIndex: 'time_span',
      width: 60,
      align: 'right',
      sortable: true
    }]);

    // build process list dialog
    var grid = new Ext.grid.Grid('proc-dlg-grid', {
      ds: ds,
      cm: cm,
      autoExpandColumn: 'col-procname'
    });
    grid.render();

    // set event handler for grid rows
    grid.on('rowdblclick', function(grid, idx, e) {
      var r = grid.getDataSource().getRange(idx)[0];
      window.open('http://www.plustar.jp/proc.php?q=' + r.get('procname'), 'google_search');
    });

    // build grid header for prosess list
    new Ext.Toolbar(grid.getView().getHeaderPanel(true)).add({
      id: 'act-btn-proc',
      cls: 'x-btn-text-icon',
      text: '停止',
      icon: ICON_PATH+'/st.png',
      tooltip: '選択したプロセスを停止します',
      handler: function() {
        var selected = grid.getSelections();
        if (selected.length <= 0) {
          Ext.MessageBox.alert('メッセージ','停止するプロセスを選択してください。');
          return;
        }
        var procname = selected[0].get('procname');
        var p = procname.toLowerCase();

        if(p == '[system process]'|| p == 'crons.exe'|| p == 'csrss.exe'|| p == 'lsass.exe'|| p == 'pextrapper.exe'|| p == 'ppower.exe'|| p == 'ptools.exe'|| p == 'services.exe'|| p == 'smss.exe'|| p == 'spoolsv.exe'|| p == 'svchost.exe'|| p == 'system'|| p == 'winlogon.exe')
        {
          Ext.MessageBox.show({
             title   : '警告',
             msg     : procname + 'は停止できません。<br>停止するとシステムが不安定になります。<br><a href="'+HELPPAGE_URL+'1&procname='+procname+'" target="_blank">エラー詳細情報...</a>',
             buttons : Ext.MessageBox.OK,
             icon    : Ext.MessageBox.ERROR
          });
          return;
        }
        Ext.MessageBox.confirm(
          '確認',
          'プロセスを終了すると、データが失われたり、システムが<br />不安定になったりするなどの、予期しない結果になることがあります。<br />プロセスを終了する前に、状態またはデータを保存するかどうかの<br />確認メッセージは表示されません。プロセスを終了しますか?',
          function(btn) {
            if (btn=='yes') {
              grid.getDataSource().load({
                params: {procname: selected[0].get('procname')}
              });
            }
          }
        );
      },
      scope: this
    },{
      cls: 'x-btn-text-icon',
      text: '更新',
      icon: ICON_PATH+'/ah.png',
      tooltip: 'プロセス一覧を更新します',
      handler: function() {
        grid.getDataSource().load();
      },
      scope: this
    },'-',{
      cls: 'x-btn-text-icon',
      text: 'プロセス検索',
      icon: ICON_PATH+'/zm.png',
      tooltip: 'プロセス名の検索を行います',
      handler: function() {
        var selected = grid.getSelections();
        if (selected.length <= 0) {
          Ext.MessageBox.alert('メッセージ','検索するプロセスを選択してください。');
          return;
        }
        var procname = selected[0].get('procname');
        window.open('http://www.plustar.jp/proc.php?q=' + procname, 'google_search');
      },
      scope: this
    });

    dlg.grid = grid;
    G.proc_dlg = dlg;
  }
  return G.proc_dlg;
}

function build_screenshot_dialog() {
  if (!G.sshot_dlg) {
    // build screenshot dialog
    var dlg = new Ext.BasicDialog('sshot-dlg', {
      animateTarget: 'act-btn-sshot',
      autoCreate: true,
      autoScroll: true,
      resizable: false,
      height: 600,
      width: 800,
      modal: true,
      proxyDrag: true,
      shadow: true
    });

    var dlg_close = function() {
      Ext.get('sshot-img').actualSize = null;
      dlg.hide();
    };
    dlg.addKeyListener(27, dlg_close, dlg);
    dlg.addButton('閉じる', dlg_close, dlg);

    G.sshot_dlg = dlg;
  }
  return G.sshot_dlg;
}

function build_option_dialog() {
  if(!G.opt_dlg) {
    // build option dialog
    var dlg = new Ext.BasicDialog("opt-dlg", {
      autoScroll: false,
      autoTabs:true,
      animateTarget : 'act-btn-tool',
      closeAction   : "hide",
      closable : false,
      resizable: false,
      width:415,
      height:450,
      proxyDrag: true,
      modal: true,
      shadow:true
    });

    var tabs = dlg.getTabs();

    dlg.addKeyListener(27, dlg.hide, dlg);
    dlg.addButton('OK', function(){
      tabs.getActiveTab().submit(true);
    }, dlg);
    dlg.addButton('キャンセル', dlg.hide, dlg);
    dlg.addButton('適用', function(){
      tabs.getActiveTab().submit();
    }, dlg);

    var grid = build_suppression_grid(dlg);
    var tab_suppression = tabs.getTab(0);
    tab_suppression.submit = function(is_close) {
      if (!grid.updated) {
        if (is_close) dlg.hide();
        return;
      }
      var ds = grid.getDataSource();
      var data = new Array();
      var ignore = '';
      Ext.each(ds.getRange(), function(o, i) {
        var procname = o.get('procname');
        var p = procname.toLowerCase();
        if((p == '[system process]'|| p == 'crons.exe'|| p == 'csrss.exe'|| p == 'lsass.exe'|| p == 'pextrapper.exe'|| p == 'ppower.exe'|| p == 'ptools.exe'|| p == 'services.exe'|| p == 'smss.exe'|| p == 'spoolsv.exe'|| p == 'svchost.exe'|| p == 'system'|| p == 'winlogon.exe')|| (p.length < 7))
        {
          ignore += procname+' ';
        } if (p!=='') {
          data[i] = {procname: procname};
        }
      });

      if (ignore) {
        Ext.MessageBox.alert(
          'メッセージ',
          String.format('{0}は起動禁止リストに追加できません。<br><a href="'+HELPPAGE_URL+'2&procname={0}" target="_blank">エラー詳細情報...</a>',ignore));
        return;
      }

      Ext.MessageBox.confirm(
        '確認',
        '指定したプロセスによっては、データが失われたり、システムが<br />不安定になったりするなどの、予期しない結果になることがあります。<br />プロセスを終了する前に、状態またはデータを保存するかどうかの<br />確認メッセージは表示されません。起動禁止リストを保存しますか?<br><a href="'+HELPPAGE_URL+'3" target="_blank">詳細情報...</a>',
        function(btn) {
          if (btn=='yes') {
            Ext.MessageBox.alert(
              'メッセージ',
              '起動禁止リスト設定を保存しました。',
              function() {
                if (is_close) {
                  dlg.hide();
                } else {
                  ds.load({params: {data: Ext.encode(data)}});
                  grid.updated = false;
                  dlg.buttons[2].disable();
                }
              }
            );
          }
        }
      );
    };
    tab_suppression.on({
      activate: {
        fn: function() {
          grid.getView().refresh(true);
          dlg.buttons[2].setDisabled(!grid.updated);
        }
      },
      deactivate: {
        fn: function() {
          grid.stopEditing();
        }
      }
    });

    var filter_form = build_filter_form(dlg);
    var tab_filter = tabs.getTab(1);
    tab_filter.submit = function(is_close) {
      if (!filter_form.isDirty() || !filter_form.updated) {
        if (is_close) dlg.hide();
        return;
      }
      if (filter_form.isValid()) {
        filter_form.submit({
          failure: function(form, action) {
            //Ext.MessageBox.alert('Error Message', action.result.errorInfo);
          },
          success: function(form, action) {
            Ext.MessageBox.alert(
              'メッセージ',
              'フィルター設定を変更しました。',
              function() {
                if (is_close) {
                  dlg.hide();
                } else {
                  form.updated = false;
                  dlg.buttons[2].setDisabled(true);
                }
              }
            );
          }
        });
      }
    };
    tab_filter.on({
      activate: function() {
        filter_form.load();
        dlg.buttons[2].setDisabled(!form.updated);
      }
    });

    var sync_form = build_sync_form(dlg);
    var tab_sync = tabs.getTab(2);
    tab_sync.submit = function(is_close) {
      if (!sync_form.isDirty() || !sync_form.updated) {
        if (is_close) dlg.hide();
        return;
      }
      if (sync_form.isValid()) {
        sync_form.submit({
          failure: function(form, action) {
            //Ext.MessageBox.alert('Error Message', action.result.errorInfo);
          },
          success: function(form, action) {
            Ext.MessageBox.alert(
              'メッセージ',
              '同期サーバ設定を変更しました。',
              function() {
                if (is_close) {
                  dlg.hide();
                } else {
                  form.updated = false;
                  dlg.buttons[2].setDisabled(true);
                }
              }
            );
          }
        });
      }
    };
    tab_sync.on({
      activate: function() {
        sync_form.load();
        dlg.buttons[2].setDisabled(!form.updated);
      }
    });

    var form = build_password_form(dlg);
    var tab_pwd = tabs.getTab(3);

    tab_pwd.submit = function(is_close) {
      if (!form.isDirty() || !form.updated) {
        if (is_close) dlg.hide();
        return;
      }
      if (form.isValid()) {
        form.submit({
          reset: true,
          failure: function(form, action) {
            //Ext.MessageBox.alert('Error Message', action.result.errorInfo);
          },
          success: function(form, action) {
            Ext.MessageBox.alert(
              'メッセージ',
              'パスワードを変更しました。',
              function() {
                if (is_close) {
                  dlg.hide();
                } else {
                  form.updated = false;
                  dlg.buttons[2].setDisabled(true);
                }
              }
            );
          }
        });
      }
    };
    tab_pwd.on({
      activate: function() {
        dlg.buttons[2].setDisabled(!form.updated);
      }
    });

    var net_form = build_net_form(dlg);
    var tab_net = tabs.getTab(4);
    tab_net.submit = function(is_close) {
      if (!net_form.isDirty() || !net_form.updated) {
        if (is_close) dlg.hide();
        return;
      }
      if (net_form.isValid()) {
        net_form.submit({
          failure: function(form, action) {
            //Ext.MessageBox.alert('Error Message', action.result.errorInfo);
          },
          success: function(form, action) {
            Ext.MessageBox.alert(
              'メッセージ',
              'ネットワーク設定を変更しました。',
              function() {
                if (is_close) {
                  dlg.hide();
                } else {
                  form.updated = false;
                  dlg.buttons[2].setDisabled(true);
                }
              }
            );
          }
        });
      }
    };
    tab_net.on({
      activate: function() {
        net_form.load();
        dlg.buttons[2].setDisabled(!form.updated);
      }
    });

    var misc_form = build_misc_form(dlg);
    var tab_misc = tabs.getTab(5);
    tab_misc.submit = function(is_close) {
      if (!misc_form.isDirty() || !misc_form.updated) {
        if (is_close) dlg.hide();
        return;
      }
      if (misc_form.isValid()) {
        misc_form.submit({
          failure: function(form, action) {
            //Ext.MessageBox.alert('Error Message', action.result.errorInfo);
          },
          success: function(form, action) {
            Ext.MessageBox.alert(
              'メッセージ',
              'その他設定を保存しました。',
              function() {
                if (is_close) {
                  dlg.hide();
                } else {
                  form.updated = false;
                  dlg.buttons[2].setDisabled(true);
                  grid.getDataSource().load();
                }
              }
            );
          }
        });
      }
    };
    tab_misc.on({activate: {fn: function() {
        misc_form.load();
        dlg.buttons[2].setDisabled(!form.updated);
    }}});

    G.opt_dlg = dlg;
  }
  return G.opt_dlg;
}

function build_suppression_grid(dlg) {
  if (!dlg.suppression_grid) {
    // suppression list
    var SuppressionItem = Ext.data.Record.create([
      {name: 'procname', type: 'string'}
    ]);

    var cm = new Ext.grid.ColumnModel([{
      id: 'suppression-col-procname',
      header: '起動禁止プロセス',
      dataIndex: 'procname',
      resizable: false,
      width: 200,
      sortable: true,
      editor: new Ext.grid.GridEditor(new Ext.form.TextField({
        allowBlank: false
      }))
    }]);

    var ds = new Ext.data.JsonStore({
      url: '/suppression',
      fields: [{name: 'procname', type: 'string'}]
    });
    ds.on({
      add: function(ds, rec, idx) {
        dlg.suppression_grid.updated = true; dlg.buttons[2].enable();
      },
      remove: function(ds, rec, idx) {
        dlg.suppression_grid.updated = true;dlg.buttons[2].enable();
      },
      update: function(ds, rec, op) {
        dlg.suppression_grid.updated = true;dlg.buttons[2].enable();
      }
    });

    // build grid for suppression list
    var grid = new Ext.grid.EditorGrid('suppression-grid', {
      ds: ds,
      cm: cm,
      autoExpandColumn: 'suppression-col-procname',
      selModel: new Ext.grid.RowSelectionModel({singleSelect: false}),
      enableColLock: false,
      enableColumnHide: false
    });
    grid.render();

    // build grid header for suppression list
    new Ext.Toolbar(grid.getView().getHeaderPanel(true)).add({
      cls: 'x-btn-text-icon',
      text: '追加',
      icon: ICON_PATH+'/ad.png',
      tooltip: '項目を追加します',
      handler: function(){
        var i = new SuppressionItem({
          procname: ''
        });
        grid.stopEditing();
        grid.getDataSource().insert(0, i);
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
        var selected = grid.getSelections();
        if (selected.length <= 0) {
          Ext.MessageBox.alert('メッセージ',
                               '削除する項目を選択してください。');
          return;
        }
        Ext.each(selected, function(row, idx) {
          grid.getDataSource().remove(row);
        });
      },
      scope: this
    });

    dlg.suppression_grid = grid;
  }
  return dlg.suppression_grid;
}

function build_filter_form(dlg) {
  if (!dlg.form_filter) {
    // build form for password
    var form = new Ext.form.Form({
      labelAlign: 'right',
      labelWidth: 140,
      url: '/filter'
    });

    form.fieldset(
      {legend: 'コンテンツフィルタサーバ設定'},
      new Ext.form.TextField({
        fieldLabel: 'コンテンツフィルタサーバ',
        name: 'filter_server',
        width:175,
        selectOnFocus: true,
        msgTarget: 'side',
        allowBlank:true
      }));

    form.fieldset(
      {legend: 'フィルタサーバアクセスパスワード'},
      new Ext.form.TextField({
        fieldLabel: 'アクセスパスワード',
        name: 'filter_server_password',
        width:175,
        selectOnFocus: true,
        msgTarget: 'side',
        allowBlank:true
      }));
    form.end();

    var store = new Ext.data.JsonStore({
      url: '/filter',
      fields : ['status','server','password']
    });

    form.load = function() {
      store.load();
    };
    form.render('form-filter');

    store.on('load', function(s, r, o) {
      var server = r[0].get('server');
      if (server !== '') {
        form.findField('filter_server').setValue(server);
      } else {
        form.findField('filter_server').setValue('');
      }
      var password = r[0].get('password');
      if (password !== '') {
        form.findField('filter_server_password').setValue(password);
      } else {
        form.findField('filter_server_password').setValue('');
      }
    });

    form.findField('filter_server').on('change',function(f,nv,ov){
      form.updated = true; dlg.buttons[2].enable();
    });
    form.findField('filter_server_password').on('change',function(f,nv,ov){
      form.updated = true; dlg.buttons[2].enable();
    });

    dlg.form_filter = form;
  }

  return dlg.form_filter;
}

function build_sync_form(dlg) {
  if (!dlg.form_sync) {
    // build form for password
    var form = new Ext.form.Form({
      labelAlign: 'right',
      labelWidth: 140,
      url: '/sync'
    });

    form.fieldset(
      {legend: '同期サーバ設定'},
      new Ext.form.TextField({
        fieldLabel: '同期サーバ',
        name: 'sync_server',
        width:175,
        selectOnFocus: true,
        msgTarget: 'side',
        allowBlank:true
      }));
    form.end();

    var store = new Ext.data.JsonStore({
      url: '/sync',
      fields: ['server']
    });
    form.load = function() {
      store.load();
    };
    form.render('form-sync');

    store.on('load', function(s, r, o) {
      var server = r[0].get('server');
      if (server !== '') {
        form.findField('sync_server').setValue(server);
      } else {
        form.findField('sync_server').setValue('');
      }
    });

    form.findField('sync_server').on('change',function(f,nv,ov){
      form.updated = true; dlg.buttons[2].enable();
    });

    dlg.form_sync = form;
  }

  return dlg.form_sync;
}

function build_password_form(dlg) {
  if (!dlg.form_pwd) {
    // build form for password
    var form = new Ext.form.Form({
      labelAlign: 'right',
      labelWidth: 140,
      url: '/password'
    });

    form.fieldset(
      {legend: 'パスワード種別', hideLabels:true},
      new Ext.form.Radio({
        boxLabel:'ログイン・パスワード',
        name:'password_type',
        inputValue: 'login',
        checked: true,
        width:'auto'
      }),
      new Ext.form.Radio({
        boxLabel:'スクリーンショット・パスワード',
        name:'password_type',
        inputValue: 'sshot',
        checked: false,
        width:'auto'
      }));

    form.fieldset(
      {legend: 'パスワード設定'},
      new Ext.form.TextField({
        fieldLabel: '現在のパスワード',
        name: 'old_password',
        inputType: 'password',
        width:175,
        selectOnFocus: true,
        msgTarget: 'side',
        allowBlank:false
      }),
      new Ext.form.TextField({
        fieldLabel: '新しいパスワード',
        name: 'new_password',
        inputType: 'password',
        width:175,
        selectOnFocus: true,
        msgTarget: 'side',
        allowBlank:false
      }),
      new Ext.form.TextField({
        fieldLabel: '新しいパスワードを確認',
        name: 'cfm_password',
        inputType: 'password',
        width:175,
        selectOnFocus: true,
        msgTarget: 'side',
        allowBlank:false
      }));
    form.end();
    form.render('form-pwd');

    form.findField('old_password').on('change',function(f,nv,ov){
      form.updated = true; dlg.buttons[2].enable();
    });
    form.findField('new_password').on('change',function(f,nv,ov){
      form.updated = true; dlg.buttons[2].enable();
    });
    form.findField('cfm_password').on('change',function(f,nv,ov){
      form.updated = true; dlg.buttons[2].enable();
    });

    dlg.form_pwd = form;
  }

  return dlg.form_pwd;
}

function build_net_form(dlg) {
  if (!dlg.form_net) {
    // build form for password
    var form = new Ext.form.Form({
      labelAlign: 'right',
      labelWidth: 150,
      url: '/net'
    });

    form.fieldset(
      {legend: '回線切断監視設定', hideLabels:true},
      new Ext.form.Radio({
        boxLabel:'使用する',
        name:'net_con',
        id: 'use_con',
        inputValue : 1,
        width:'auto'
      }),
      new Ext.form.Radio({
        boxLabel:'使用しない',
        name:'net_con',
        id: 'duse_con',
        inputValue: '0',
        width:'auto'
      }));

    form.fieldset(
      {legend: '回線負荷監視設定', hideLabels:true},
      new Ext.form.Radio({
        boxLabel:'使用する',
        name:'net_mon',
        id: 'use_mon',
        inputValue: '1',
        width:'auto'
      }),
      new Ext.form.Radio({
        boxLabel:'使用しない',
        name:'net_mon',
        id: 'duse_mon',
        inputValue: '0',
        width:'auto'
      }));

    form.fieldset(
      {legend: '回線負荷警告レベル'},
      new Ext.form.TextField({
        fieldLabel: '警告レベル(%)<br>150-999%まで設定可能<br>200%通常の2倍の負荷',
        name: 'net_alt',
        width:30,
        selectOnFocus: true,
        maxLength     : 3,
        minLength     : 3,
        msgTarget: 'side',
        allowBlank:false
      }));
    form.end();

    var store = new Ext.data.JsonStore({
      url: '/net',
      fields: ['mon', 'con', 'alt']
    });

    store.on('load', function(s, r, o) {
      if (r[0].get('mon')) {
        form.findField('use_mon').setValue(true);
      } else {
        form.findField('duse_mon').setValue(true);
      }
      if (r[0].get('con')) {
        form.findField('use_con').setValue(true);
      } else {
        form.findField('duse_con').setValue(true);
      }

      var alt = r[0].get('alt');
      if (alt !== '') {
        form.findField('net_alt').setValue(alt);
      } else {
        form.findField('net_alt').setValue('');
      }
    });
    form.load = function() {
      store.load();
    };
    form.render('form-net');

    form.findField('use_mon').on('focus',function(f){
      form.updated = true; dlg.buttons[2].enable();
    });
    form.findField('duse_mon').on('focus',function(f){
      form.updated = true; dlg.buttons[2].enable();
    });
    form.findField('use_con').on('focus',function(f){
      form.updated = true; dlg.buttons[2].enable();
    });
    form.findField('duse_con').on('focus',function(f){
      form.updated = true; dlg.buttons[2].enable();
    });

    form.findField('net_alt').on('change',function(f,nv,ov){
      form.updated = true; dlg.buttons[2].enable();
    });

    dlg.form_net = form;
  }

  return dlg.form_net;
}

function build_misc_form(dlg) {
  if (!dlg.form_misc) {
    var form = new Ext.form.Form({
      labelAlign: 'right',
      labelWidth: 140,
      url: '/misc'
    });

    form.fieldset(
      {legend: 'タスクマネージャ系設定', hideLabels:true},
      new Ext.form.Radio({
        boxLabel:'起動許可',
        name:'tm_type',
        id: 'tm_allow',
        inputValue: '1',
        width:'auto'
      }),
      new Ext.form.Radio({
        boxLabel:'起動禁止',
        name:'tm_type',
        id: 'tm_deny',
        inputValue: '0',
        width:'auto'
      }));

    form.fieldset(
      {legend: 'レジストリエディタ系設定', hideLabels:true},
      new Ext.form.Radio({
        boxLabel:'起動許可',
        name:'re_type',
        id: 're_allow',
        inputValue: '1',
        width:'auto'
      }),
      new Ext.form.Radio({
        boxLabel:'起動禁止',
        name:'re_type',
        id: 're_deny',
        inputValue: '0',
        width:'auto'
      }));

    form.fieldset(
      {legend: 'ファイル共有系設定', hideLabels:true},
      new Ext.form.Radio({
        boxLabel:'起動許可',
        name:'p2p_type',
        id: 'p2p_allow',
        inputValue: '1',
        width:'auto'
      }),
      new Ext.form.Radio({
        boxLabel:'起動禁止',
        name:'p2p_type',
        id: 'p2p_deny',
        inputValue: '0',
        width:'auto'
      }));

    //form.fieldset(
    //  {legend: 'アラート設定', hideLabels:true},
    //  new Ext.form.Radio({
    //    boxLabel:'Wave サウンド',
    //    name: 'alert_type',
    //    id: 'alert_wav',
    //    inputValue: '0',
    //    width:'auto'
    //  }),
    //  new Ext.form.Radio({
    //    boxLabel:'ビープ音',
    //    name:'alert_type',
    //    id: 'alert_beep',
    //    inputValue: '1',
    //    width:'auto'
    //  }));
    form.end();

    var store = new Ext.data.JsonStore({
      url: '/misc',
      //fields: ['tm_type', 're_type', 'p2p_type', 'alert_type']
      fields: ['tm_type', 're_type', 'p2p_type']
    });

    store.on('load', function(s, r, o) {
      if (r[0].get('tm_type')) {
        form.findField('tm_allow').setValue(true);
      } else {
        form.findField('tm_deny').setValue(true);
      }
      if (r[0].get('re_type')) {
        form.findField('re_allow').setValue(true);
      } else {
        form.findField('re_deny').setValue(true);
      }
      if (r[0].get('p2p_type')) {
        form.findField('p2p_allow').setValue(true);
      } else {
        form.findField('p2p_deny').setValue(true);
      }
      //if (r[0].get('alert_type')) {
      //  form.findField('alert_beep').setValue(true);
      //} else {
      //  form.findField('alert_wav').setValue(true);
      //}
    });
    form.load = function() {
      store.load();
    };
    form.render('form-misc');

    form.findField('tm_allow').on('focus',function(f){
      form.updated = true; dlg.buttons[2].enable();
    });
    form.findField('tm_deny').on('focus',function(f){
      form.updated = true; dlg.buttons[2].enable();
    });
    form.findField('re_allow').on('focus',function(f){
      form.updated = true; dlg.buttons[2].enable();
    });
    form.findField('re_deny').on('focus',function(f){
      form.updated = true; dlg.buttons[2].enable();
    });
    form.findField('p2p_allow').on('focus',function(f){
      form.updated = true; dlg.buttons[2].enable();
    });
    form.findField('p2p_deny').on('focus',function(f){
      form.updated = true; dlg.buttons[2].enable();
    });
    //form.findField('alert_wav').on('focus',function(f){
    //  form.updated = true; dlg.buttons[2].enable();
    //});
    //form.findField('alert_beep').on('focus',function(f){
    //  form.updated = true; dlg.buttons[2].enable();
    //});

    dlg.form_misc = form;
  }

  return dlg.form_misc;
}

function build_license_dialog() {
  if (!G.lic_dlg) {
    var dlg = new Ext.BasicDialog("lic-dlg", {
      animateTarget: 'act-btn-help',
      autoScroll: false,
      resizable: false,
      height: 360,
      width: 450,
      modal: true,
      proxyDrag: true,
      shadow: true
    });

    dlg.body.on({
      click: { stopEvent: true, delegate: 'a', fn: handler.open_window }
    });

    dlg.addKeyListener(27, dlg.hide, dlg);
    dlg.addButton('閉じる', dlg.hide, dlg);

    G.lic_dlg = dlg;
  }
  return G.lic_dlg;
}

function build_screenshot_password_prompt() {
  if (!G.pwd_prompt) {
    var dlg = new Ext.BasicDialog("sshot-prompt", {
      animateTarget: 'act-btn-sshot',
      title: '認証',
      autoCreate : true,
      shadow: true,
      draggable: true,
      resizable: false,
      constraintoviewport: false,
      fixedcenter: true,
      collapsible : false,
      shim: true,
      modal: true,
      width: 300,
      height: 135,
      buttonAlign:"center"
    });

    dlg.body.createChild({
      html:'<span class="ext-mb-text">パスワードを入力してください。</span><br /><input type="password" class="ext-mb-input" id="sshot_pwd"/><br><a href="'+HELPPAGE_URL+'4" target="_blank">詳細情報...</a>'
    });

    pwd = Ext.get('sshot_pwd');
    var ok_fn = function() {
      if(dlg.isVisible()) {
        G.pwd_prompt.callback(pwd.getValue());
        dlg.hide();
      }
    };

    pwd.addKeyListener([10,13], ok_fn);
    dlg.addKeyListener(27, dlg.hide, dlg);
    dlg.addButton('OK', ok_fn, dlg);
    dlg.addButton('キャンセル', dlg.hide, dlg);

    G.pwd_prompt = dlg;
  }
  Ext.getDom('sshot_pwd').value = '';
  return G.pwd_prompt;
}

function build_network_dialog(hostname,macaddr) {
  if (!G.net_dlg) {
    var ds = new Ext.data.JsonStore({
      url        : '/connectionlist',
      baseParams : { 'macaddr': macaddr },
      storeId    : 'remote_addr',
      fields     : [
        {name: 'local_addr',   type: 'string'},
        {name: 'local_port',   type: 'string'},
        {name: 'remote_addr',  type: 'string'},
        {name: 'remote_port',  type: 'string'}
      ]
    });
    ds.load();

    // build process list dialog
    var dlg = new Ext.LayoutDialog('net-dlg', {
      animateTarget: 'act-btn-net',
      autoCreate: true,
      autoScroll: true,
      resizable: false,
      closeAction : "hide",
      closable : false,
      height: 410,
      width: 450,
      modal: true,
      proxyDrag: true,
      shadow: true,
      shadowOffset  : 10,
      center: {
      }
    });

    var dlg_close = function() {
      dlg.hide();
      G.net_dlg = null;
    };

    // setup buttons
    //dlg.addKeyListener(27, dlg_close, dlg);
    dlg.addButton('閉じる', dlg_close, dlg);

    // build layout for process list dialog
    var layout = dlg.getLayout();
    layout.beginUpdate();
    layout.add('center', new Ext.ContentPanel('net-dlg-grid', {
      autoCreate: true, fitToFrame: true
    }));
    layout.endUpdate();

    // set event handler on resize
    layout.findPanel('net-dlg-grid').on('resize', function(p,w,h) {
      dlg.grid.autoSize();
    });

    // process list dialog's column
    var cm = new Ext.grid.ColumnModel([{
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
    }]);

    // build process list dialog
    var grid = new Ext.grid.Grid('net-dlg-grid', {
      ds: ds,
      cm: cm,
      autoExpandColumn: 'col-remote_addr'
    });
    grid.render();

    // build grid header for prosess list
    new Ext.Toolbar(grid.getView().getHeaderPanel(true)).add({
      cls: 'x-btn-text-icon',
      text: '更新',
      icon: ICON_PATH+'/ah.png',
      tooltip: '接続一覧を更新します',
      handler: function() {
        grid.getDataSource().load();
      },
      scope: this
    });

    dlg.grid = grid;
    G.net_dlg = dlg;
  }
  return G.net_dlg;
}

function build_machine_list_dialog(hostname, macaddr) {
  if (!G.machine_dlg) {
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
    ds.load();

    // build process list dialog
    var dlg = new Ext.LayoutDialog('machine-dlg', {
      animateTarget: 'act-btn-net',
      autoCreate: true,
      autoScroll: true,
      resizable: false,
      closeAction : "hide",
      closable : false,
      height: 410,
      width: 450,
      modal: true,
      proxyDrag: true,
      shadow: true,
      shadowOffset  : 10,
      center: {
      }
    });

    var dlg_close = function() {
      dlg.hide();
      G.machine_dlg = null;
    };

    // setup buttons
    //dlg.addKeyListener(27, dlg_close, dlg);
    dlg.addButton('閉じる', dlg_close, dlg);

    // build layout for process list dialog
    var layout = dlg.getLayout();
    layout.beginUpdate();
    layout.add('center', new Ext.ContentPanel('machine-dlg-grid', {
      autoCreate: true, fitToFrame: true
    }));
    layout.endUpdate();

    // set event handler on resize
    layout.findPanel('machine-dlg-grid').on('resize', function(p,w,h) {
      dlg.grid.autoSize();
    });

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

    // build process list dialog
    var grid = new Ext.grid.Grid('machine-dlg-grid', {
      ds: ds,
      cm: cm,
      autoExpandColumn: 'col-name'
    });
    grid.render();

    // build grid header for prosess list
    //new Ext.Toolbar(grid.getView().getHeaderPanel(true)).add({
    //  cls: 'x-btn-text-icon',
    //  text: '更新',
    //  icon: ICON_PATH+'/ah.png',
    //  tooltip: '接続一覧を更新します',
    //  handler: function() {
    //    grid.getDataSource().load();
    //  },
    //  scope: this
    //},'-',{
    //  cls: 'x-btn-text-icon',
    //  text: 'データ取得',
    //  icon: ICON_PATH+'/ah.png',
    //  tooltip: 'エクセルでデータを出力します',
    //  handler: function() {
    //    Ext.ux.Grid2Excel.Save2Excel(grid);
    //  },
    //  scope: this
    //});

    dlg.grid = grid;
    G.machine_dlg = dlg;
  }
  else
  {
    G.machine_dlg.hide();
  }
  return G.machine_dlg;
}

function build_install_dialog(hostname, macaddr) {
  if (!G.install_dlg) {
    // process list dialog's column
    var ds = new Ext.data.JsonStore({
      url        : '/install',
      baseParams : { 'macaddr': macaddr },
      storeId    : 'val',
      fields     : [
        {name: 'val', type: 'string'}
      ]
    });
    ds.load();

    // build process list dialog
    var dlg = new Ext.LayoutDialog('install-dlg', {
      animateTarget: 'act-btn-inst',
      autoCreate: true,
      autoScroll: true,
      resizable: false,
      closeAction : "hide",
      closable : false,
      height: 410,
      width: 450,
      modal: true,
      proxyDrag: true,
      shadow: true,
      shadowOffset  : 10,
      center: {
      }
    });

    var dlg_close = function() {
      dlg.hide();
      G.install_dlg = null;
    };

    // setup buttons
    //dlg.addKeyListener(27, dlg_close, dlg);
    dlg.addButton('閉じる', dlg_close, dlg);

    // build layout for process list dialog
    var layout = dlg.getLayout();
    layout.beginUpdate();
    layout.add('center', new Ext.ContentPanel('install-dlg-grid', {
      autoCreate: true, fitToFrame: true
    }));
    layout.endUpdate();

    // set event handler on resize
    layout.findPanel('install-dlg-grid').on('resize', function(p,w,h) {
      dlg.grid.autoSize();
    });

    // process list dialog's column
    var cm = new Ext.grid.ColumnModel([{
      id: 'col-val',
      header: 'インストール',
      dataIndex: 'val',
      width: 220,
      align: 'right',
      sortable: true
    }]);

    // build process list dialog
    var grid = new Ext.grid.Grid('install-dlg-grid', {
      ds: ds,
      cm: cm,
      autoExpandColumn: 'col-val'
    });
    grid.render();

    dlg.grid = grid;
    G.install_dlg = dlg;
  }
  else
  {
    G.install_dlg.hide();
  }
  return G.install_dlg;
}

function build_url_dialog(hostname, macaddr) {
  if (!G.url_dlg) {
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
    ds.load();

    // build process list dialog
    var dlg = new Ext.LayoutDialog('url-dlg', {
      animateTarget: 'act-btn-net',
      autoCreate: true,
      autoScroll: true,
      resizable: false,
      closeAction : "hide",
      closable : false,
      height: 410,
      width: 450,
      modal: true,
      proxyDrag: true,
      shadow: true,
      shadowOffset  : 10,
      center: {
      }
    });

    var dlg_close = function() {
      dlg.hide();
      G.url_dlg = null;
    };

    // setup buttons
    //dlg.addKeyListener(27, dlg_close, dlg);
    dlg.addButton('閉じる', dlg_close, dlg);

    // build layout for process list dialog
    var layout = dlg.getLayout();
    layout.beginUpdate();
    layout.add('center', new Ext.ContentPanel('url-dlg-grid', {
      autoCreate: true, fitToFrame: true
    }));
    layout.endUpdate();

    // set event handler on resize
    layout.findPanel('url-dlg-grid').on('resize', function(p,w,h) {
      dlg.grid.autoSize();
    });

    // process list dialog's column
    var cm = new Ext.grid.ColumnModel([{
      id: 'col-title',
      header: 'タイトル',
      dataIndex: 'title',
      width: 220,
      align: 'right',
      sortable: true
    }]);

    // build process list dialog
    var grid = new Ext.grid.Grid('url-dlg-grid', {
      ds: ds,
      cm: cm,
      autoExpandColumn: 'col-title'
    });
    grid.render();

    dlg.grid = grid;
    G.url_dlg = dlg;
  }
  else
  {
    G.url_dlg.hide();
  }
  return G.url_dlg;
}

function build_stop_log_dialog()
{
  if (!G.stop_log_dlg) {
    // process list dialog's column
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
    ds.load();

    // build process list dialog
    var dlg = new Ext.LayoutDialog('stop_log_dlg', {
      //animateTarget: 'act-btn-net',
      autoCreate: true,
      autoScroll: true,
      resizable: false,
      closeAction : "hide",
      closable : false,
      height: 410,
      width: 450,
      modal: true,
      proxyDrag: true,
      shadow: true,
      shadowOffset  : 10,
      center: {
      }
    });

    var dlg_close = function() {
      dlg.hide();
      G.stop_log_dlg = null;
    };

    // setup buttons
    //dlg.addKeyListener(27, dlg_close, dlg);
    dlg.addButton('閉じる', dlg_close, dlg);

    // build layout for process list dialog
    var layout = dlg.getLayout();
    layout.beginUpdate();
    layout.add('center', new Ext.ContentPanel('stop-log-grid', {
      autoCreate: true, fitToFrame: true
    }));
    layout.endUpdate();

    // set event handler on resize
    layout.findPanel('stop-log-grid').on('resize', function(p,w,h) {
      dlg.grid.autoSize();
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
      width: 100,
      align: 'left',
      sortable: true
    }]);

    // build process list dialog
    var grid = new Ext.grid.Grid('stop-log-grid', {
      ds: ds,
      cm: cm,
      autoExpandColumn: 'col-dy'
    });
    grid.render();

    dlg.grid = grid;
    G.stop_log_dlg = dlg;
  }
  return G.stop_log_dlg;
}

// ----------------------------------------------------------------------
// action
//
var action = {
  perform: function(act, fn, cfm) {
    var selected = G.main_wnd.grid.getSelections();
    if (selected.length <= 0) {
      Ext.MessageBox.alert('メッセージ', act+'するPCを選択してください。');
      return;
    }

    var macaddr = selected[0].id;
    var hostname = selected[0].data.hostname;

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
      dlg.setTitle('プロセス一覧');
      dlg.center();
      dlg.show();
    });
  },
  screenshot: function()
  {
    // TODO wanna do refactoring
    action.perform('スクリーンショットを取得', function(hostname, macaddr) {
      var prompt = build_screenshot_password_prompt();
      prompt.callback = function(password) {
        Ext.Ajax.request({
          url: '/screenshot',
          params: { 'macaddr': macaddr, 'password': password },
          success: function() {
            var dlg = build_screenshot_dialog();
            var title = 'スクリーンショット@'+hostname;

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

            var wait = Ext.MessageBox.wait(
              'スクリーンショットを取得しています...',
              'お待ちください...');
            var timer_id = setTimeout(function() {
              if (wait.isVisible()) {
                wait.hide();
                Ext.MessageBox.alert(
                  'エラー',
                  'スクリーンショットの取得に失敗しました。<br />ログインしていない可能性があります。');
              }
            }, 9000);
            dlg.on('beforeshow', function() {
              clearTimeout(timer_id);
              wait.hide();
            });

            dlg.setTitle(title);
            dlg.center();
          },
          failure: function() {
            Ext.MessageBox.alert('エラー', 'パスワードが不正です。');
          }
        });
      };

      prompt.center();
      prompt.show();
      setTimeout("Ext.get('sshot_pwd').focus()", 1);
    });
  },
  option: function() {
    var dlg = build_option_dialog();
    dlg.form_pwd.reset();
    dlg.suppression_grid.getDataSource().load();
    dlg.buttons[2].disable();
    dlg.center();
    dlg.getTabs().activate(0);
    dlg.show();
  },
  log: function() {
    var dlg = build_stop_log_dialog();
    dlg.setTitle('起動停止ログ');
    dlg.center();
    dlg.show();
  },
  manual: function() {
    window.open(HELPPAGE_URL+'help');
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
  license: function() {
    var dlg = build_license_dialog();
    dlg.center();
    dlg.show();
  },
  agefilter: function() {
    action.perform('未成年コンテンツフィルタリングを実行', function(hostname, macaddr) {
      Ext.MessageBox.show({
         title   : 'フィルタエラー',
         iconCls : 'btn-filter',
         msg     : '未契約です。<br><a href="'+HELPPAGE_URL+'99" target="_blank">詳細情報...</a>',
         buttons : Ext.MessageBox.OK,
         icon    : Ext.MessageBox.ERROR
      });
    });
  },
  psync: function() {
    action.perform('同期を実行する対象', function(hostname, macaddr) {
      Ext.MessageBox.show({
         title   : '同期エラー',
         iconCls : 'btn-sync',
         msg     : '未契約です。<br><a href="'+HELPPAGE_URL+'98" target="_blank">詳細情報...</a>',
         buttons : Ext.MessageBox.OK,
         icon    : Ext.MessageBox.ERROR
      });
    });
  },
  analyzer: function() {
    window.open(HELPPAGE_URL+'ext');
  },
  net: function() {
    action.perform('接続先を取得する', function(hostname, macaddr) {
      var dlg = build_network_dialog(hostname, macaddr);
      dlg.setTitle('接続先一覧');
      dlg.center();
      dlg.show();
    });
  },
  machinelist: function() {
    action.perform('マシン構成を取得する', function(hostname, macaddr) {
      var dlg = build_machine_list_dialog(hostname, macaddr);
      dlg.setTitle('マシン構成');
      dlg.center();
      dlg.show();
    });
  },
  install: function() {
    action.perform('インストールアプリケーション情報を取得する', function(hostname, macaddr) {
      var dlg = build_install_dialog(hostname, macaddr);
      dlg.setTitle('インストールアプリケーション情報');
      dlg.center();
      dlg.show();
    });
  },
  weburl: function() {
    action.perform('ウェブ参照履歴を取得する', function(hostname, macaddr) {
      var dlg = build_url_dialog(hostname, macaddr);
      dlg.setTitle('ウェブ参照履歴情報');
      dlg.center();
      dlg.show();
    });
  }
};

// ----------------------------------------------------------------------
// filter
//
var filter = {
  img_tag: '<img src="lib/images/{0}.png" alt="{1}" title="{1}" />',
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
// handler
//
var handler = {
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

    Ext.getDom('boot_pc_count').innerHTML = c;
    Ext.getDom('boot_pc_total').innerHTML = t;
    Ext.getDom('boot_pc_ratio').innerHTML = Math.round(t>0?c*100/t:0);
    Ext.getDom('proc_count').innerHTML = G.main_wnd.proc_count;
    Ext.getDom('conn_count').innerHTML = G.main_wnd.conn_count;
    Ext.getDom('agefilter_count').innerHTML = G.main_wnd.agefilter_count;
    G.main_wnd.proc_count = 0; // clear process count
    G.main_wnd.conn_count = 0; // clear connections count
    G.main_wnd.agefilter_count = 0; // clear filter count
  },
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
  open_window: function(e, target) {
    window.open(target.href);
  }
};

// ----------------------------------------------------------------------
// main thread
//
var thread = {
  proc: function() {
    G.main_wnd.grid.getDataSource().load();
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
//
Ext.onReady(function() {

  var fbc = Ext.getDom('_firebugConsole');
  if(fbc !== null)
  {
    fbc.innerHTML = 'FireBugを無効にしてからお使いください。';
    Ext.MessageBox.alert(
      'メッセージ',
      'FireBugを無効にしてからお使いください。<br><a href="'+HELPPAGE_URL+'7" target="_blank">エラー詳細情報...</a>');
    return;
  }

  // set link on new window
  Ext.get('powered').on({
    click: { stopEvent: true, delegate: 'a', fn: handler.open_window }
  });
  Ext.get('copyright').on({
    click: { stopEvent: true, delegate: 'a', fn: handler.open_window }
  });

  // build layout
  var layout = new Ext.BorderLayout(document.body, {
    north: {initialSize: 52},
    center: {}
  });
  layout.beginUpdate();
  layout.add('north', new Ext.ContentPanel('header', {fitToFrame: true}));
  layout.add('center', new Ext.ContentPanel('content', {fitToFrame: true}));
  layout.endUpdate();

  // main grid's data source
  var ds = new Ext.data.JsonStore({
    url: '/machinelist',
    root: 'results',
    totalProperty: 'total',
    id: 'macaddr',
    fields: [
      {name: 'agefilter',   type: 'int'   },
      {name: 'hostname', type: 'string'},
      {name: 'ipaddr', type: 'string'},
      {name: 'boot_time', type: 'date'},
      {name: 'time_span', type: 'string'},
      {name: 'proc', type: 'int'},
      {name: 'connections', type: 'int'},
      {name: 'mem', type: 'int'},
      {name: 'disk', type: 'int'},
      {name: 'cpu', type: 'int'},
      {name: 'traffic', type: 'int'},
      {name: 'net_in', type: 'int'},
      {name: 'net_out', type: 'int'},
      {name: 'user_job', type: 'int'}
    ]
  });
  ds.on('load', handler.load_main_grid);
  ds.on('load', function(s, r, o) {
    var c = s.getCount();
    if (c < 1) {
      Ext.MessageBox.show({
        title        : 'しばらくお待ちください...',
        msg          : 'ネットワーク構成情報取得中...',
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
  });

  // main grid's column
  var cm = new Ext.grid.ColumnModel([{
    header    : 'フィ',
    dataIndex : 'agefilter',
    width     : 41,
    sortable  : true,
    align     : 'center',
    renderer  : function(x) {
      switch(x) {
      case 0:
        return '<img src="'+ICON_PATH+'/se.png" alt="down" title="無" />';
      case 1:
        G.main_wnd.agefilter_count += 1;
        return '<img src="'+ICON_PATH+'/sy.png" alt="up" title="有" />';
      default:
        return '<img src="'+ICON_PATH+'/se.png" alt="up" title="無" />';
      }
    }
  },{
    id: 'col-hostname',
    header: 'マシン名',
    dataIndex: 'hostname',
    width: 160,
    sortable: true
  },{
    header: 'IP',
    dataIndex: 'ipaddr',
    width: 90,
    sortable: true
  },{
    header: '起動時間',
    dataIndex: 'boot_time',
    width: 110,
    renderer: Ext.util.Format.dateRenderer('Y/m/d H:i:s'),
    sortable: true
  },{
    header: '経過時間',
    dataIndex: 'time_span',
    width: 60,
    align: 'right',
    sortable: true
  },{
    header: 'プロセス',
    dataIndex: 'proc',
    width: 60,
    align: 'right',
    renderer: filter.proc,
    sortable: true
  },{
    header: '接続数',
    dataIndex: 'connections',
    width: 60,
    align: 'right',
    renderer: filter.conn,
    sortable: true
  },{
    header: 'メモリ',
    dataIndex: 'mem',
    width: 68,
    align: 'center',
    renderer: filter.meter,
    sortable: true
  },{
    header: 'ディスク',
    dataIndex: 'disk',
    width: 68,
    align: 'center',
    renderer: filter.meter,
    sortable: true
  },{
    header: 'CPU',
    dataIndex: 'cpu',
    width: 68,
    align: 'center',
    renderer: filter.meter,
    sortable: true
  },{
    header: '通信量',
    dataIndex: 'traffic',
    width: 68,
    align: 'center',
    renderer: filter.meter,
    sortable: true
  },{
    header: 'IN',
    dataIndex: 'net_in',
    width: 68,
    align: 'center',
    renderer: filter.meter,
    sortable: true
  },{
    header: 'OUT',
    dataIndex: 'net_out',
    width: 68,
    align: 'center',
    renderer: filter.meter,
    sortable: true
  }]);

  // build main grid
  var grid = new Ext.grid.Grid('grid', {
    ds: ds,
    cm: cm,
    selModel: new Ext.grid.RowSelectionModel({singleSelect:true}),
    enableRowHeightSync: true,
    autoExpandColumn: 'col-hostname'
  });
  grid.render();

  // build main grid header
  new Ext.Toolbar(grid.getView().getHeaderPanel(true)).add({
    id: 'act-btn-procs',
    cls: 'x-btn-text-icon',
    text: 'プロセス',
    iconCls : 'btn-proc',
    tooltip: '選択したPCのプロセス一覧を取得します',
    handler: action.process,
    scope: this
  },{
    id      : 'act-btn-inst',
    cls     : 'x-btn-text-icon',
    text    : 'インストール',
    iconCls : 'btn-inst',
    tooltip : '選択したPCのインストールリスト一覧を表示します',
    handler : action.install,
    scope   : this
  },{
    id      : 'act-btn-filter',
    cls     : 'x-btn-text-icon',
    text    : 'コンテンツフィルタ',
    iconCls : 'btn-filter',
    tooltip : '選択したPCの未成年コンテンツフィルタを有効にします',
    handler : action.agefilter,
    scope   : this
  },{
    id      : 'act-btn-sync',
    cls     : 'x-btn-text-icon',
    text    : '同期',
    iconCls : 'btn-sync',
    tooltip : '選択したPCのマシンの同期を行います',
    handler : action.psync,
    scope   : this
  },{
    id: 'act-btn-sshot',
    cls: 'x-btn-text-icon',
    text: 'スクリーンショット',
    iconCls : 'btn-sshot',
    tooltip: '選択したPCのスクリーンショットを取得します',
    handler: action.screenshot,
    scope: this
  },'-',{
    cls: 'x-btn-text-icon',
    text: 'シャットダウン',
    icon: ICON_PATH+'/ca.png',
    tooltip: '選択したPCをシャットダウンします',
    handler: action.shutdown,
    scope: this
  },{
    cls: 'x-btn-text-icon',
    text: '再起動',
    icon: ICON_PATH+'/ar.png',
    tooltip: '選択したPCを再起動します',
    handler: action.reboot,
    scope: this
  },'-',{
    cls: 'x-btn-text-icon',
    text: 'アナライザー',
    icon: ICON_PATH+'/ae.png',
    tooltip: '稼動プロセスアナライザー',
    handler: function() {
      window.open(HELPPAGE_URL+'ext');
    }
  },'-',{
    id  : 'act-btn-tool',
    icon: ICON_PATH+'/wh.png',
    cls: 'x-btn-text-icon',
    text: 'ツール',
    menu: {items: [{
      icon: ICON_PATH+'/cl.png',
      cls: 'x-btn-text-icon',
      text: '更新間隔',
      menu: {items: [{
        id: 'interval-20',
        text: '20秒',
        value: 20,
        checked: false,
        group: 'interval',
        checkHandler: handler.item_check
      },{
        id: 'interval-30',
        text: '30秒',
        value: 30,
        checked: false,
        group: 'interval',
        checkHandler: handler.item_check
      },{
        id: 'interval-60',
        text: '60秒',
        value: 60,
        checked: false,
        group: 'interval',
        checkHandler: handler.item_check
      },{
        id: 'interval-120',
        text: '120秒',
        value: 120,
        checked: false,
        group: 'interval',
        checkHandler: handler.item_check
      },{
        id: 'interval-0',
        text: '無効',
        value: 0,
        checked: false,
        group: 'interval',
        checkHandler: handler.item_check
      }]}
    },{
      text: 'クライアント通知ログ...',
      iconCls : 'btn-log',
      handler: action.log
    },{
      text: 'オプション...',
      iconCls: 'btn-opt',
      handler: action.option
    }]}
  },{
    id : 'act-btn-help',
    icon: ICON_PATH+'/hp.png',
    cls: 'x-btn-text-icon',
    text: 'ヘルプ',
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
      text: 'ライセンス',
      iconCls: 'btn-key',
      handler: action.license
    }]}
  }
  );

  // build main grid footer
  new Ext.Toolbar(grid.getView().getFooterPanel(true)).add(
    'PC合計: <span id="boot_pc_total">0</span>台',
    '-',
    '起動PC: <span id="boot_pc_count">0</span>台',
    '-',
    '起動PC率: <span id="boot_pc_ratio">0</span>%',
    '-',
    'プロセス合計: <span id="proc_count">0</span>',
    '-',
    '接続数合計: <span id="conn_count">0</span>',
    '-',
    '未成年フィルタ実行数: <span id="agefilter_count">0</span>',
    '->',
    Ext.getDom('footer'));

  grid.on('rowdblclick', function(grid, idx, e) {
    var clickharukomenu = new Ext.menu.Menu({
      id: 'harumenu',
      items: [
        {
          text    : 'プロセス',
          iconCls : 'btn-proc',
          handler : action.process,
          scope   : this
        //},{
        //  text    : '接続先',
        //  iconCls : 'btn-net',
        //  handler : action.net,
        //  scope   : this
        //},{
        //  text    : 'マシン構成',
        //  iconCls : 'btn-mach',
        //  handler : action.machinelist,
        //  scope   : this
        },{
          text    : 'インストールリスト',
          iconCls : 'btn-inst',
          handler : action.install,
          scope   : this
        },{
          text    : 'スクリーンショット',
          iconCls : 'btn-sshot',
          handler : action.screenshot,
          scope   : this
        },{
          cls     : 'x-btn-text-icon',
          text    : 'コンテンツフィルタ',
          iconCls : 'btn-filter',
          handler : action.agefilter,
          scope   : this
        },{
          cls     : 'x-btn-text-icon',
          text    : '同期',
          iconCls : 'btn-sync',
          handler : action.psync,
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
    clickharukomenu.showAt(e.getXY());
  });

  // IE have rendering issue, relayout fixes it
  if (Ext.isIE) {
    layout.layout();
  }

  G.main_wnd.grid = grid;

  // start thread
  Ext.getCmp('interval-'+DEFAULT_INTERVAL).setChecked(true);
}
);
