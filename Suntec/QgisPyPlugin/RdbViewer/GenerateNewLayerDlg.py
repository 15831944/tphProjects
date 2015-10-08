import os
import psycopg2
from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox

FeatureTypeList = \
[
    """spotguide""",
    """lane"""
]

FORM_CLASS, _ = uic.loadUiType(os.path.join(
    os.path.dirname(__file__), 'GenerateNewLayerDlgDesign.ui'))

class GenerateNewLayerDlg(QtGui.QDialog, FORM_CLASS):
    def __init__(self, parent=None):
        super(GenerateNewLayerDlg, self).__init__(parent)
        self.setupUi(self)
        self.lineEditHost.setText("""192.168.10.14""")
        self.lineEditPort.setText("""5432""")
        self.lineEditDbName.setText("""post13_NI_Dummy_CI""")
        self.lineEditUser.setText("""postgres""")
        self.lineEditPassword.setText("""pset123456""")
        for oneText in FeatureTypeList:
            self.comboBoxSelectFeature.addItem(oneText)
        self.connect(self.comboBoxSelectFeature, 
                     QtCore.SIGNAL('activated(QString)'), 
                     self.comboBoxSelectFeatureChanged);
        self.lineEditNewLayerName.setText("""temp_spotguide_nodes""")
        self.pushButtonGenerateLayer.clicked.connect(self.generateLayer)
        self.pushButtonTestConnect.clicked.connect(self.testConnect)

    # generate a new layer with specified name.   
    def generateLayer(self):
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
            errMsg = """Input arguments not correct.\n"""+\
                        """host: %s\n"""+\
                        """port: %s\n"""+\
                        """dbname: %s\n"""+\
                        """user: %s\n"""+\
                        """password: %s\n"""+\
                        """newLayerName: %s\n"""+\
                        """feature: %s\n""" % \
                        (host, port, dbname, user, 
                         password, newLayerName, featureType)
            QMessageBox.information(self, "error", errMsg[0])
            return
        sqlcmd = """drop table if exists %s;\n"""+\
                 """create table %s \n"""+\
                 """as \n"""+\
                 """(%s);"""
        userSqlcmd = self.getuserSqlcmd(featureType)
        sqlcmd = sqlcmd % (newLayerName, newLayerName, userSqlcmd)
        try:
            conn = psycopg2.connect('''host='%s' dbname='%s' user='%s' password='%s' ''' %\
                (host, dbname, user, password))
            pg = conn.cursor()
            pg.execute(sqlcmd)
            conn.commit()
        except Exception, ex:
            errMsg[0] = ex.message
            return

        self.removeAllLayersWithSpecName(errMsg, newLayerName)
        if errMsg[0] != '':
            return
        uri = QgsDataSourceURI()        
        uri.setConnection(host, port, dbname, user, password)        
        uri.setDataSource("public", newLayerName, "the_geom", "")
        layer = QgsVectorLayer(uri.uri(), newLayerName, "postgres")
        layer.name = newLayerName
        if not layer.isValid():
            errMsg[0] = """Create new layer failed. """+\
                        """host: %s, dbName: %s, user: %s, password: %s""" % \
                (host, port, dbname, user, password)
            return 
        # Add layer to the registry
        QgsMapLayerRegistry.instance().addMapLayer(layer)
        return

    def testConnect(self):
        inti = 0
        inti += 1
        return

    def comboBoxSelectFeatureChanged(self, txt):
        return
           
    def getuserSqlcmd(self, featureType):
        if featureType == """spotguide""":
            return \
"""select a.*, b.data as pattern_data, c.data as arrow_data, d.the_geom as the_geom
from 
rdb_guideinfo_spotguidepoint as a
left join rdb_guideinfo_pic_blob_bytea as b
on a.pattern_id=b.gid
left join rdb_guideinfo_pic_blob_bytea as c
on a.arrow_id=c.gid
left join rdb_node as d
on a.node_id=d.node_id"""
        elif  featureType == """lane""":
            return \
"""select a.*, b.the_geom
from
rdb_guideinfo_lane as a
left join rdb_node as b
on a.node_id=b.node_id
"""
        else:
            return ''



