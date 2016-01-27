#encoding=utf-8

class NodeDataItem(object):
    def __init__(self):
        self.gid = None
        self.node_id = None
        self.node_id_t = None
        self.extend_flag = None
        self.link_num = None
        self.branches = None
        return

    def initFromRecord(self, record):
        self.gid = record[0]
        self.node_id = record[1]
        self.node_id_t = record[2]
        self.extend_flag = record[3]
        self.link_num = record[4]
        self.branches = record[5]
        return

    def initNormal(self, gid, node_id, node_id_t, extend_flag, link_num, branches):
        self.gid = gid
        self.node_id = node_id
        self.node_id_t = node_id_t
        self.extend_flag = extend_flag
        self.link_num = link_num
        self.branches = branches
        return

    def getKeyString(self):
        return """%s(%s)""" % (self.node_id, self.gid)

    def toString(self):
        return """xxxxxxxxxxx"""


