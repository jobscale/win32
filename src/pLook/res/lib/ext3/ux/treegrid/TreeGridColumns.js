/*
 * Ext JS Library 3.2.1
 * Copyright(c) 2006-2010 Ext JS, Inc.
 * licensing@extjs.com
 * http://www.extjs.com/license
 */
(function(){Ext.override(Ext.list.Column,{init:function(){var b=Ext.data.Types,a=this.sortType;if(this.type){if(Ext.isString(this.type)){this.type=Ext.data.Types[this.type.toUpperCase()]||b.AUTO}}else{this.type=b.AUTO}if(Ext.isString(a)){this.sortType=Ext.data.SortTypes[a]}else{if(Ext.isEmpty(a)){this.sortType=this.type.sortType}}}});Ext.tree.Column=Ext.extend(Ext.list.Column,{});Ext.tree.NumberColumn=Ext.extend(Ext.list.NumberColumn,{});Ext.tree.DateColumn=Ext.extend(Ext.list.DateColumn,{});Ext.tree.BooleanColumn=Ext.extend(Ext.list.BooleanColumn,{});Ext.reg("tgcolumn",Ext.tree.Column);Ext.reg("tgnumbercolumn",Ext.tree.NumberColumn);Ext.reg("tgdatecolumn",Ext.tree.DateColumn);Ext.reg("tgbooleancolumn",Ext.tree.BooleanColumn)})();