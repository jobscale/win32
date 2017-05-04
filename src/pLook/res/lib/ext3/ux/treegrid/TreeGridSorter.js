/*
 * Ext JS Library 3.2.1
 * Copyright(c) 2006-2010 Ext JS, Inc.
 * licensing@extjs.com
 * http://www.extjs.com/license
 */
Ext.ns("Ext.ux.tree");Ext.ux.tree.TreeGridSorter=Ext.extend(Ext.tree.TreeSorter,{sortClasses:["sort-asc","sort-desc"],sortAscText:"Sort Ascending",sortDescText:"Sort Descending",constructor:function(a,b){if(!Ext.isObject(b)){b={property:a.columns[0].dataIndex||"text",folderSort:true}}Ext.ux.tree.TreeGridSorter.superclass.constructor.apply(this,arguments);this.tree=a;a.on("headerclick",this.onHeaderClick,this);a.ddAppendOnly=true;me=this;this.defaultSortFn=function(j,i){var f=me.dir&&me.dir.toLowerCase()=="desc";var c=me.property||"text";var e=me.sortType;var g=me.folderSort;var h=me.caseSensitive===true;var d=me.leafAttr||"leaf";if(g){if(j.attributes[d]&&!i.attributes[d]){return 1}if(!j.attributes[d]&&i.attributes[d]){return -1}}var l=e?e(j):(h?j.attributes[c]:j.attributes[c].toUpperCase());var k=e?e(i):(h?i.attributes[c]:i.attributes[c].toUpperCase());if(l<k){return f?+1:-1}else{if(l>k){return f?-1:+1}else{return 0}}};a.on("afterrender",this.onAfterTreeRender,this,{single:true});a.on("headermenuclick",this.onHeaderMenuClick,this)},onAfterTreeRender:function(){if(this.tree.hmenu){this.tree.hmenu.insert(0,{itemId:"asc",text:this.sortAscText,cls:"xg-hmenu-sort-asc"},{itemId:"desc",text:this.sortDescText,cls:"xg-hmenu-sort-desc"})}this.updateSortIcon(0,"asc")},onHeaderMenuClick:function(d,b,a){if(b==="asc"||b==="desc"){this.onHeaderClick(d,null,a);return false}},onHeaderClick:function(e,b,a){if(e&&!this.tree.headersDisabled){var d=this;d.property=e.dataIndex;d.dir=e.dir=(e.dir==="desc"?"asc":"desc");d.sortType=e.sortType;d.caseSensitive===Ext.isBoolean(e.caseSensitive)?e.caseSensitive:this.caseSensitive;d.sortFn=e.sortFn||this.defaultSortFn;this.tree.root.cascade(function(c){if(!c.isLeaf()){d.updateSort(d.tree,c)}});this.updateSortIcon(a,e.dir)}},updateSortIcon:function(b,a){var d=this.sortClasses;var c=this.tree.innerHd.select("td").removeClass(d);c.item(b).addClass(d[a=="desc"?1:0])}});