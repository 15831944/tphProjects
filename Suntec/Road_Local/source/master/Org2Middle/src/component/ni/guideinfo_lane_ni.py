import component.component_base


class comp_guideinfo_lane_ni(component.component_base.comp_base):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Guideinfo_Lane')

    def _DoCreateTable(self):
        if self.CreateTable2('lane_tbl') == -1:
            return -1
        return 0

    def _DoCreateIndex(self):
        'create index.'
        return 0

    def _DoCreateFunction(self):
        self.CreateFunction2('mid_get_line_turn')
        self.CreateFunction2('mid_make_arrowinfo7_from_link_ni')
        self.CreateFunction2('mid_make_arrowinfo_ni')
        self.CreateFunction2('mid_get_passlinkcount_ni')
        self.CreateFunction2('mid_make_passlinkid')
        return 0

    def _Do(self):
        sqlcmd='''
            insert into lane_tbl
            (
                  id,inlinkid,nodeid,outlinkid,lanenum,laneinfo,arrowinfo,lanenuml,lanenumr,passlink_cnt,passlid,buslaneinfo)
            (
              SELECT   gid::integer as id
                   , inlinkid::bigint
                   , nodeid::bigint
                   , outlinkid::bigint
                   , lanenum::smallint
                   , substr(laneinfo,1,lanenum::smallint) as laneinfo
                   , case when arrowinfo<>'7' then mid_make_arrowinfo_ni(arrowinfo) else mid_make_arrowinfo7_from_link_ni(inlinkid ,nodeid ,passlid ,outlinkid) end
                   , lanenuml::smallint
                   , lanenumr::smallint
                   , mid_get_passlinkcount_ni(mid_make_passlinkid(inlinkid,passlid,passlid2,outlinkid))
                   , mid_make_passlinkid(inlinkid,passlid,passlid2,outlinkid)
                   , buslane
              FROM org_ln
              order by gid
            )
            '''
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd= '''
            delete from lane_Tbl where laneinfo=replace(laneinfo,'1','0')
                '''
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0


