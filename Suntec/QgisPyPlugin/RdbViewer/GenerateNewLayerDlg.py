import os
import psycopg2
from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox
from qgis.core import QgsVectorLayer, QgsMapLayerRegistry, QgsDataSourceURI

####################################################################
guideTypeList = \
[
    """rdb_guideinfo_spotguidepoint""", #0
    """rdb_guideinfo_signpost""",
    """rdb_guideinfo_lane""",
    """rdb_link_regulation""",
    """rdb_guideinfo_building_structure""",
    """rdb_guideinfo_caution""", #5
    """rdb_guideinfo_crossname""",
    """rdb_guideinfo_forceguide""",
    """rdb_guideinfo_hook_turn""",
    """rdb_guideinfo_natural_guidence""",
    """rdb_guideinfo_road_structure""", #10
    """rdb_guideinfo_safety_zone""",
    """rdb_guideinfo_safetyalert""",
    """rdb_guideinfo_signpost_uc""",
    """rdb_guideinfo_towardname""" #14
]
####################################################################
strGenerateLayerSpotguide = \
"""
drop table if exists %s;
create table %s 
as 
(
    select a.*, b.image_id as pattern_name, b.data as pattern_dat, 
            c.image_id as arrow_name, c.data as arrow_dat, 
            st_union(array[e.the_geom, f.the_geom]) as the_geom
    from 
    rdb_guideinfo_spotguidepoint as a
    left join rdb_guideinfo_pic_blob_bytea as b
    on a.pattern_id=b.gid
    left join rdb_guideinfo_pic_blob_bytea as c
    on a.arrow_id=c.gid
    left join rdb_link as e
    on a.in_link_id=e.link_id
    left join rdb_link as f
    on a.out_link_id=f.link_id
);
"""
strGenerateLayerSignpost = \
"""
drop table if exists %s;
create table %s 
as 
(
    select a.*, b.image_id as pattern_name, b.data as pattern_dat, 
            c.image_id as arrow_name, c.data as arrow_dat, 
            st_union(array[e.the_geom, f.the_geom]) as the_geom
    from 
    rdb_guideinfo_signpost as a
    left join rdb_guideinfo_pic_blob_bytea as b
    on a.pattern_id=b.gid
    left join rdb_guideinfo_pic_blob_bytea as c
    on a.arrow_id=c.gid
    left join rdb_link as e
    on a.in_link_id=e.link_id
    left join rdb_link as f
    on a.out_link_id=f.link_id
);
"""

strGenerateLayerLane = \
"""
drop table if exists %s;
create table %s 
as 
(
    select a.*, st_union(array[c.the_geom, d.the_geom]) as the_geom
    from
    rdb_guideinfo_lane as a
    left join rdb_link as c
    on a.in_link_id=c.link_id
    left join rdb_link as d
    on a.out_link_id=d.link_id
);
"""

strGenerateLayerRegulation = \
"""
drop table if exists temp_regulation_link_seqnr;
select record_no, 
       unnest(string_to_array(key_string, ',')::bigint[]) as link_id, 
       generate_series(1, array_upper(string_to_array(key_string, ','), 1)) as seqnr
into temp_regulation_link_seqnr
from rdb_link_regulation;

create index temp_regulation_link_seqnr_record_no_idx
on temp_regulation_link_seqnr
using btree
(record_no);
create index temp_regulation_link_seqnr_link_id_idx
on temp_regulation_link_seqnr
using btree
(link_id);

drop table if exists temp_regulation_link_list_geom;
select a.record_no, st_union(array_agg(b.the_geom)) as the_geom
into temp_regulation_link_list_geom
from
temp_regulation_link_seqnr as a
left join rdb_link as b
on a.link_id=b.link_id
group by record_no;

create index temp_regulation_link_list_geom_record_no_idx
on temp_regulation_link_list_geom
using btree
(record_no);

drop table if exists %s;
create table %s 
as
(
    select a.record_no, a.regulation_id, a.regulation_type, a.is_seasonal, a.first_link_id, 
           a.first_link_id_t, a.last_link_id, a.last_link_dir, a.last_link_id_t, a.link_num, 
           a.key_string, a.order_id, b.the_geom
    from 
    rdb_link_regulation as a
    left join temp_regulation_link_list_geom as b
    on a.record_no=b.record_no
);"""

strGenerateLayerBuildingStructure = \
"""
drop table if exists %s;
create table %s
as
(
    select a.*, b.the_geom 
    from 
    rdb_guideinfo_building_structure as a
    left join rdb_node as b
    on a.node_id=b.node_id
);"""

strGenerateLayerCaution = \
"""
drop table if exists %s;
create table %s
as
(
    select a.*, b.the_geom
    from
    rdb_guideinfo_caution as a
    left join rdb_link as b
    on a.in_link_id=b.link_id
);"""

strGenerateLayerCondition = \
"""
drop table if exists %s;
create table %s
as
(
    select a.*, b.the_geom
    from
    rdb_guideinfo_caution as a
    left join rdb_link as b
    on a.in_link_id=b.link_id
);"""

strGenerateLayerCrossname = \
"""
drop table if exists %s;
create table %s
as
(
    select a.*, st_union(b.the_geom, c.the_geom) as the_geom
    from
    rdb_guideinfo_crossname as a
    left join rdb_link as b
    on a.in_link_id=b.link_id
    left join rdb_link as c
    on a.out_link_id=c.link_id
);"""

strGenerateLayerForceguide = \
"""
drop table if exists %s;
create table %s
as
(
    select a.*, st_union(b.the_geom, c.the_geom) as the_geom
    from
    rdb_guideinfo_forceguide as a
    left join rdb_link as b
    on a.in_link_id=b.link_id
    left join rdb_link as c
    on a.out_link_id=c.link_id
);"""

strGenerateLayerHookTurn = \
"""
drop table if exists %s;
create table %s
as
(
    select a.*, st_union(b.the_geom, c.the_geom) as the_geom
    from
    rdb_guideinfo_hook_turn as a
    left join rdb_link as b
    on a.in_link_id=b.link_id
    left join rdb_link as c
    on a.out_link_id=c.link_id
);"""
strGenerateLayerNaturalGuidence = \
"""
drop table if exists %s;
create table %s
as
(
    select a.*, st_union(b.the_geom, c.the_geom) as the_geom
    from
    rdb_guideinfo_natural_guidence as a
    left join rdb_link as b
    on a.in_link_id=b.link_id
    left join rdb_link as c
    on a.out_link_id=c.link_id
);"""
strGenerateLayerRoadStructure = \
"""
drop table if exists %s;
create table %s
as
(
    select a.*, st_union(b.the_geom, c.the_geom) as the_geom
    from
    rdb_guideinfo_road_structure as a
    left join rdb_link as b
    on a.in_link_id=b.link_id
    left join rdb_link as c
    on a.out_link_id=c.link_id
);"""
strGenerateLayerSaftyZone = \
"""
drop table if exists %s;
create table %s
as
(
    select a.*, b.the_geom
    from
    rdb_guideinfo_safety_zone as a
    left join rdb_link as b
    on a.link_id=b.link_id
);"""
strGenerateLayerSaftyAlert = \
"""
drop table if exists %s;
create table %s
as
(
    select a.*, b.the_geom
    from
    rdb_guideinfo_safetyalert as a
    left join rdb_link as b
    on a.link_id=b.link_id
);"""
strGenerateLayerSignpostUc = \
"""
drop table if exists %s;
create table %s
as
(
    select a.*, st_union(b.the_geom, c.the_geom) as the_geom
    from
    rdb_guideinfo_signpost_uc as a
    left join rdb_link as b
    on a.in_link_id=b.link_id
    left join rdb_link as c
    on a.out_link_id=c.link_id
);"""
strGenerateLayerTowardname = \
"""
drop table if exists %s;
create table %s
as
(
    select a.*, st_union(b.the_geom, c.the_geom) as the_geom
    from
    rdb_guideinfo_towardname as a
    left join rdb_link as b
    on a.in_link_id=b.link_id
    left join rdb_link as c
    on a.out_link_id=c.link_id
);"""

sqlcmdDict = \
{
    guideTypeList[0] : strGenerateLayerSpotguide,
    guideTypeList[1] : strGenerateLayerSignpost,
    guideTypeList[2] : strGenerateLayerLane,
    guideTypeList[3] : strGenerateLayerRegulation,
    guideTypeList[4] : strGenerateLayerBuildingStructure,
    guideTypeList[5] : strGenerateLayerCaution,
    guideTypeList[6] : strGenerateLayerCrossname,
    guideTypeList[7] : strGenerateLayerForceguide,
    guideTypeList[8] : strGenerateLayerHookTurn,
    guideTypeList[9] : strGenerateLayerNaturalGuidence,
    guideTypeList[10] : strGenerateLayerRoadStructure,
    guideTypeList[11] : strGenerateLayerSaftyZone,
    guideTypeList[12] : strGenerateLayerSaftyAlert,
    guideTypeList[13] : strGenerateLayerSignpostUc,
    guideTypeList[14] : strGenerateLayerTowardname
}
####################################################################
tempTblNameDict = \
{
    guideTypeList[0] : """temp_spotguide_geom""",
    guideTypeList[1] : """temp_signpost_geom""",
    guideTypeList[2] : """temp_lane_geom""",
    guideTypeList[3] : """temp_link_regulation_geom""",
    guideTypeList[4] : """temp_building_structure_geom""",
    guideTypeList[5] : """temp_caution_geom""",
    guideTypeList[6] : """temp_crossname_geom""",
    guideTypeList[7] : """temp_forceguide_geom""",
    guideTypeList[8] : """temp_hook_turn_geom""",
    guideTypeList[9] : """temp_natural_guidence_geom""",
    guideTypeList[10] : """temp_road_structure_geom""",
    guideTypeList[11] : """temp_safety_zone_geom""",
    guideTypeList[12] : """temp_saftyalert_geom""",
    guideTypeList[13] : """temp_signpost_uc_geom""",
    guideTypeList[14] : """temp_towardname_geom"""
}
####################################################################

FORM_CLASS, _ = uic.loadUiType(os.path.join(
    os.path.dirname(__file__), 'GenerateNewLayerDlgDesign.ui'))

class GenerateNewLayerDlg(QtGui.QDialog, FORM_CLASS):
    def __init__(self, parent=None):
        super(GenerateNewLayerDlg, self).__init__(parent)
        self.setupUi(self)
        self.lineEditHost.setText("""192.168.10.14""")
        self.lineEditPort.setText("""5432""")
        self.lineEditDbName.setText("""post13_MEA8_RDF_CI""")
        self.lineEditUser.setText("""postgres""")
        self.lineEditPassword.setText("""pset123456""")
        for oneType in guideTypeList:
            self.comboBoxSelectFeature.addItem(oneType)
        self.connect(self.comboBoxSelectFeature, 
                     QtCore.SIGNAL('activated(QString)'), 
                     self.comboBoxSelectFeatureChanged);
        self.lineEditNewLayerName.setText("""temp_spotguide_geom""")
        self.pushButtonGenerateLayer.clicked.connect(self.onPushButtonGenerateLayer)
        self.pushButtonTestConnect.clicked.connect(self.onPushButtonTestConnect)

    def comboBoxSelectFeatureChanged(self):
        strFeatureType = self.comboBoxSelectFeature.currentText()
        self.lineEditNewLayerName.setText(tempTblNameDict[strFeatureType])
        return

    # generate a new layer with specified name.   
    def onPushButtonGenerateLayer(self):
        host = self.lineEditHost.text()
        port = self.lineEditPort.text()
        dbname = self.lineEditDbName.text()
        user = self.lineEditUser.text()
        password = self.lineEditPassword.text()
        featureType = self.comboBoxSelectFeature.currentText()
        newLayerName = self.lineEditNewLayerName.text()
        if host is None or \
           port is None or \
           dbname is None or \
           user is None or \
           password is None or \
           newLayerName is None or \
           featureType is None:
            errMsg += """error:\n"""
            errMsg += """Input arguments not correct.\n"""
            errMsg += """host: %s\n""" % host
            errMsg += """port: %s\n""" % port
            errMsg += """dbname: %s\n""" % dbname
            errMsg += """user: %s\n""" % user
            errMsg += """password: %s\n""" % password
            errMsg += """newLayerName: %s\n""" % newLayerName
            errMsg += """feature: %s\n""" % featureType
            QMessageBox.information(self, "Generate New Layer", errMsg[0])
            return

        sqlcmd = (sqlcmdDict[featureType]) % (newLayerName, newLayerName)
        try:
            conn = psycopg2.connect('''host='%s' dbname='%s' user='%s' password='%s' ''' %\
                (host, dbname, user, password))
            pg = conn.cursor()
            pg.execute(sqlcmd)
            conn.commit()
        except Exception, ex:
            QMessageBox.information(self, "Generate New Layer", """error:\n%s"""%ex.message)
            return

        errMsg = ['']
        self.removeAllLayersWithSpecName(errMsg, newLayerName)
        if errMsg[0] != '':
            QMessageBox.information(self, "Generate New Layer", """error:\n%s"""%errMsg[0])
            return
        uri = QgsDataSourceURI()
        uri.setConnection(host, port, dbname, user, password)
        uri.setDataSource("public", newLayerName, "the_geom", "")
        layer = QgsVectorLayer(uri.uri(), newLayerName, "postgres")
        layer.name = newLayerName
        if not layer.isValid():
            errMsg[0] = """error:\nCreate new layer failed. """+\
                        """host: %s, port: %s, dbName: %s, user: %s, password: %s""" % \
                        (host, port, dbname, user, password)
            return 
        # Add layer to the registry
        QgsMapLayerRegistry.instance().addMapLayer(layer)
        return

    def onPushButtonTestConnect(self):
        host = self.lineEditHost.text()
        port = self.lineEditPort.text()
        dbname = self.lineEditDbName.text()
        user = self.lineEditUser.text()
        password = self.lineEditPassword.text()
        featureType = self.comboBoxSelectFeature.currentText()
        newLayerName = self.lineEditNewLayerName.text()
        if host is None or \
           port is None or \
           dbname is None or \
           user is None or \
           password is None or \
           newLayerName is None or \
           featureType is None:
            errMsg += """error:\n"""
            errMsg += """Input arguments not correct.\n"""
            errMsg += """host: %s\n""" % host
            errMsg += """port: %s\n""" % port
            errMsg += """dbname: %s\n""" % dbname
            errMsg += """user: %s\n""" % user
            errMsg += """password: %s\n""" % password
            errMsg += """newLayerName: %s\n""" % newLayerName
            errMsg += """feature: %s\n""" % featureType
            QMessageBox.information(self, "Generate New Layer", errMsg[0])
            return

        try:
            conn = psycopg2.connect('''host='%s' dbname='%s' user='%s' password='%s' ''' %\
                (host, dbname, user, password))
            pg = conn.cursor()
            pg.execute("""select * from pg_stat_user_tables limit 1""")
        except Exception, ex:
            QMessageBox.information(self, "Generate New Layer", """error:\n%s"""%ex.message)
            return
        QMessageBox.information(self, "Generate New Layer", """connected.""")
        return

    # make sure errMsg is a 'string array' which has at least one element.
    def removeAllLayersWithSpecName(self, errMsg, specLayerName):
        layerList = QgsMapLayerRegistry.instance().mapLayersByName(specLayerName)
        for oneLayer in layerList:
            QgsMapLayerRegistry.instance().removeMapLayer(oneLayer.id())
        return
    




