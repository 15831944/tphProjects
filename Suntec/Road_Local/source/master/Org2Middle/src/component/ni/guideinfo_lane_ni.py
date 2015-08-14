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
        self.CreateFunction2('reverse_str')
        return 0

    def _Do(self):
        self._createtable()
        self._reverselaneinfo()
        self._makeexclusiveflag()
    
    def _createtable(self):
        
        sqlcmd='''
            insert into lane_tbl
            (
                  id,inlinkid,nodeid,outlinkid,lanenum,laneinfo,arrowinfo,lanenuml,lanenumr,passlink_cnt,passlid,buslaneinfo)
            (
              SELECT   gid::integer as id
                   , inlinkid::bigint
                   , (case when b.old_node_id is null then a.nodeid else b.new_node_id end)::bigint
                   , outlinkid::bigint
                   , lanenum::smallint
                   , substr(laneinfo,1,lanenum::smallint) as laneinfo
                   , case when arrowinfo<>'7' then mid_make_arrowinfo_ni(arrowinfo) else mid_make_arrowinfo7_from_link_ni(inlinkid ,nodeid ,passlid ,passlid2,outlinkid) end
                   , lanenuml::smallint
                   , lanenumr::smallint
                   , mid_get_passlinkcount_ni(mid_make_passlinkid(inlinkid,passlid,passlid2,outlinkid))
                   , mid_make_passlinkid(inlinkid,passlid,passlid2,outlinkid)
                   , buslane
              FROM org_ln a
              left join temp_node_mapping b
              on a.nodeid=b.old_node_id
              where a.laneinfo<>'0000000000000000'
              order by gid
            )
            '''
        
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        return 0
    
    def _reverselaneinfo(self):
        
        sqlcmd='''
            update lane_tbl a
            set laneinfo=reverse_str(a.laneinfo)
            from link_tbl b
            where a.inlinkid=b.link_id and b.extend_flag=1
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
    def _makeexclusiveflag(self):
        
        self.pg.CreateTable2_ByName('temp_lane_exclusive')
        self.pg.CreateIndex2_ByName('temp_lane_exclusive_gid_idx')
        sqlcmd='''
                update lane_tbl a
                set exclusive=b.exclusive
                from temp_lane_exclusive b
                where a.gid=b.gid
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()