import component.component_base

class deletedummylink_zenrin(component.component_base.comp_base):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'DeleteDummyLink')

    def _DoCreateTable(self):
        
        self.CreateTable2('temp_link_walked')
        self.CreateTable2('temp_dummy_reg')
        self.CreateTable2('temp_dummy_link_todelete')
        self.CreateTable2('temp_regulation_id')
        
    def _search(self,rec_org,rec,path):
        rec_list=[]
        path=eval(path)
        sqlcmd='''
                select distinct a.link_id,case when one_way_code=1 then case when a.s_node=%d then a.s_node else a.e_node end
                                    when one_way_code=2 then a.s_node 
                                    when one_way_code=3 then a.e_node end as s_node,
                case when one_way_code=1 then case when a.s_node=%d then a.e_node else a.s_node end 
                     when one_way_code=2 then a.e_node
                     when one_way_code=3 then a.s_node end as e_node,
                     (b.link_id is not null or a.link_type=4) as innerlink_flag 
                from temp_intersection_link a
                left join mid_dummy_link b
                on a.link_id=b.link_id
                where (one_way_code=1 and %d in (a.s_node,a.e_node)
                or (one_way_code=2 and a.s_node=%d) or (one_way_code=3 and a.e_node=%d)) and b.link_id is null
                '''
        self.pg.execute2(sqlcmd%(rec[2],rec[2],rec[2],rec[2],rec[2]))
        results=self.pg.fetchall2()
        
        for result in results:
            if result[0] in path:
                continue
            
            if not result[3]:
                rec_list.append([result[0],str(path+[result[0]])])
            else:
                rec_list.extend(self._search(rec_org,result,str(path+[result[0]])))
        return rec_list
    
    def _search_reg(self,rec_org,rec,path):
        rec_list=[]
        
        path=eval(path)
        
        sqlcmd='''
                select distinct a.link_id,case when a.one_way_code=1 then case when a.s_node=%d then a.s_node else a.e_node end 
                                      when a.one_way_code=2 then a.s_node 
                                      when a.one_way_code=3 then a.e_node end as s_node,
                case when one_way_code=1 then case when a.s_node=%d then a.e_node else a.s_node end 
                     when one_way_code=2 then a.e_node
                     when one_way_code=3 then a.s_node end as e_node,
                     a.link_type=4 as innerlink_flag
                from temp_intersection_link a
                left join (select c.regulation_id,c.linkid from regulation_item_tbl b 
                left join regulation_item_tbl c on b.regulation_id=c.regulation_id and c.seq_num=3
                where 
                %d = b.linkid and b.seq_num=1) c
                 on a.link_id=c.linkid
                left join regulation_relation_tbl d
                on c.regulation_id = d.regulation_id 
                left join mid_dummy_link e
                on a.link_id=e.link_id
                where d.cond_id is null and c.linkid is null and 
                (a.one_way_code=1 and %d in (a.s_node,a.e_node)
                 or (a.one_way_code=2 and a.s_node=%d) or (a.one_way_code=3 and a.e_node=%d))
                '''
        
        self.pg.execute2(sqlcmd%(rec[2],rec[2],rec[0],rec[2],rec[2],rec[2]))
        results=self.pg.fetchall2()
        
        for result in results:
            if result[0] in path:
                continue
            
            if not result[3]:
                rec_list.append([result[0],str(path+[result[0]])])

            else:
                rec_list.extend(self._search_reg(rec_org,result,str(path+[result[0]])))
        return rec_list

        
    def _search_reg_spe(self,rec_org,rec,path,time):
        rec_list=[]
        
        path=eval(path)
        sqlcmd='''
                select distinct a.link_id,case when a.one_way_code=1 then case when a.s_node=%d then a.s_node else a.e_node end 
                                      when a.one_way_code=2 then a.s_node 
                                      when a.one_way_code=3 then a.e_node end as s_node,
                case when one_way_code=1 then case when a.s_node=%d then a.e_node else a.s_node end 
                     when one_way_code=2 then a.e_node
                     when one_way_code=3 then a.s_node end as e_node,
                     a.link_type=4 as innerlink_flag
                from temp_intersection_link a
                left join (select c.regulation_id,c.linkid from regulation_item_tbl b 
                left join regulation_item_tbl c on b.regulation_id=c.regulation_id and c.seq_num=3
                where 
                %d = b.linkid and b.seq_num=1) c
                 on a.link_id=c.linkid
                left join regulation_relation_tbl d
                on c.regulation_id = d.regulation_id 
                left join mid_dummy_link e
                on a.link_id=e.link_id
                where (d.cond_id is null or d.cond_id = %d) and c.linkid is null and 
                (a.one_way_code=1 and %d in (a.s_node,a.e_node)
                 or (a.one_way_code=2 and a.s_node=%d) or (a.one_way_code=3 and a.e_node=%d))
                '''
        
        self.pg.execute2(sqlcmd%(rec[2],rec[2],rec[0],time,rec[2],rec[2],rec[2]))
        results=self.pg.fetchall2()
        for result in results:
            if result[0] in path:
                continue
            
            if not result[3]:
                rec_list.append([result[0],str(path+[result[0]])])

            else:
                rec_list.extend(self._search_reg_spe(rec_org,result,str(path+[result[0]]),time))
        return rec_list
    
    def _Do(self):
        
        self._createindex_for_regulation_tbl()

        self._calulatenewreg()
        self._update_link_tbl()
        self._update_node_tbl()
        self._update_regulation_tbl()
        self._update_guideinfo_tbl()
        
    def _createindex_for_regulation_tbl(self):
        
        self.CreateIndex2('regulation_relation_tbl_regulation_id_idx')
        self.CreateIndex2('regulation_relation_tbl_inlinkid_idx')
        self.CreateIndex2('regulation_relation_tbl_outlinkid_idx')
        self.CreateIndex2('regulation_relation_tbl_cond_id_idx')
        self.CreateIndex2('regulation_relation_tbl_condtype_idx')
        self.CreateIndex2('regulation_item_tbl_regulation_id_idx')
        self.CreateIndex2('regulation_item_tbl_linkid_idx')
    
    def _calulatenewreg(self):
    
        self.log.info('Begin calulate new regulation ...')
        
        
        
        sqlcmd = '''
                select link_id from mid_dummy_link-- where link_id=3320620007198
                '''      
        self.pg.execute2(sqlcmd)
        alldummylink=self.pg.fetchall2()
        sqlcmd_2='''
                select count(1) from temp_link_walked
                where link_id=%d
                    '''

        self.reg_id=0
        for i in alldummylink:
            self.pg.execute2(sqlcmd_2%i[0])
            cnt=self.pg.fetchall2()[0][0]
            if cnt==0:
                self._searchintersectionlink(i)
                allstartlink=self._searchallstartlink()
                condition_exists_flag=self._find_timerelated_regulation()
                delete_link_flag=True
                self.reg_list=[]
                for link in allstartlink:
                    path=str([link[0]])
                    
                    dict_a=dict(self._search(link,link,path))
                    result_a=map(lambda x:x[0],self._search(link,link,path))
                    
                    result_b=map(lambda x:x[0],self._search_reg(link,link,path))
                    if set(result_a)<>set(result_b):
                        
                        set_a=set(result_a)-set(result_b)
                        for i in set_a:
                            self.reg_list.append([dict_a[i],None])
                        if len(set(result_b)-set(result_a))>0:
                            delete_link_flag=False
                    if condition_exists_flag:
                        for time in self.all_time:
                            result_c=map(lambda x:x[0],self._search_reg_spe(link,link,path,time[0]))
                            if set(result_b)<>set(result_c):
                                set_a=set(result_b)-set(result_c)
                                for i in set_a:
                                    self.reg_list.append([dict_a[i],time[0]])                       
                
                if delete_link_flag:
                    self._insertnewregulation()
                    
        self.log.info('End calulate new regulation ...')    

    def _searchintersectionlink(self,i):
        sqlcmd_insert='''
                insert into temp_link_walked
                (link_id)
                values(%d)
                        '''
        link_arr=[i]
        self.pg.execute2(sqlcmd_insert%i[0])
        self.pg.commit2()
        while True:
            cnt1=0
            for j in link_arr:
                
                sqlcmd_3='''
                select b.link_id from link_tbl a 
                join link_tbl b
                on b.s_node in (a.s_node,a.e_node) or b.e_node in (a.s_node,a.e_node)
                left join temp_link_walked c
                on b.link_id=c.link_id
                where a.link_id=%d and b.link_type=4 and c.link_id is null
                        '''
                self.pg.execute2(sqlcmd_3%j)
                results=self.pg.fetchall2()
                cnt1+=len(results)
                
                for result in results:
                    link_arr.append(result)
                    self.pg.execute2(sqlcmd_insert%result)
                    self.pg.commit2()
            if cnt1==0:
                break
        self.CreateTable2('temp_intersection_link')
        sqlcmd_insertdummy='''
                insert into temp_intersection_link
                (link_id,s_node,e_node,one_way_code,link_type,the_geom)
                select link_id,s_node,e_node,one_way_code,link_type,the_geom
                from link_tbl
                where link_id=%d
                            '''
        for i in link_arr:
            self.pg.execute2(sqlcmd_insertdummy%i)
            self.pg.commit2()
        
        self.CreateTable2('temp_dummy_node')
        sqlcmd_alllinknear='''
                insert into temp_intersection_link
                (link_id,s_node,e_node,one_way_code,link_type,the_geom)
                select distinct a.link_id,a.s_node,a.e_node,a.one_way_code,a.link_type,a.the_geom from link_tbl a
                join 
                temp_intersection_link b
                on b.s_node in (a.s_node,a.e_node) or b.e_node in (a.s_node,a.e_node)
                left join temp_intersection_link c
                on a.link_id=c.link_id
                where c.link_id is null
                '''
        self.pg.execute2(sqlcmd_alllinknear)
        self.pg.commit2()
    
    def _searchallstartlink(self):
        sqlcmd_startlink='''
                select a.link_id,
                    case when one_way_code=1 then (case when b.node_id is not null then s_node else e_node end) 
                         when one_way_code=2 then s_node
                         when one_way_code=3 then e_node end as s_node,
                    case when one_way_code=1 then (case when b.node_id is not null then e_node else s_node end) 
                         when one_way_code=2 then e_node
                         when one_way_code=3 then s_node end as enodeno,
                         a.link_type=4 as innerlink_flag 
                from temp_intersection_link a
                left join temp_dummy_node b
                on a.e_node=b.node_id
                left join temp_dummy_node c
                on a.s_node=b.node_id
                where a.link_type<>4 and 
                        (one_way_code = 1 or 
                            (one_way_code=2 and b.node_id is not null) or
                            (one_way_code=3 and c.node_id is not null))
                        '''
        self.pg.execute2(sqlcmd_startlink)
        allstartlink=self.pg.fetchall2()
        return allstartlink
    
    def _insertnewregulation(self):
        
        sqlcmd_3='''
                insert into temp_dummy_link_todelete(link_id)
                select link_id from temp_intersection_link
                join mid_dummy_link using(link_id);
               
                insert into temp_regulation_id(regulation_id)
                select b.regulation_id from temp_intersection_link a
                join regulation_item_tbl b on a.link_id = b.linkid and b.seq_num = 1
                join regulation_item_tbl c on b.regulation_id=c.regulation_id and c.seq_num=3
                join temp_intersection_link d on c.linkid=d.link_id;
                    '''                
        self.pg.execute2(sqlcmd_3)
        self.pg.commit2()
        for reg in self.reg_list:
            self.reg_id+=1
            reg1=eval(reg[0])
            temp_time=reg[1]
            sqlcmd_insertreg='''
                insert into temp_dummy_reg(reg_id,link_id,seq,cond_id)
                values(%d,%d,%d,%d);'''
            sqlcmd_insertreg_null='''
                insert into temp_dummy_reg(reg_id,link_id,seq)
                values(%d,%d,%d);
                        '''
            if temp_time:
                for idx,link_id in enumerate(reg1):
                    self.pg.execute2(sqlcmd_insertreg%(self.reg_id,link_id,idx+1,temp_time))
            else:
                for idx,link_id in enumerate(reg1):
                    self.pg.execute2(sqlcmd_insertreg_null%(self.reg_id,link_id,idx+1))
        self.pg.commit2()
    
    def _find_timerelated_regulation(self):
        
        sqlcmd_reg='''
                        
                select count(1) from regulation_relation_tbl a
                join regulation_item_tbl b
                on a.regulation_id=b.regulation_id and b.seq_num=1
                join regulation_item_tbl c
                on a.regulation_id=c.regulation_id and c.seq_num=3
                join temp_intersection_link d on b.linkid=d.link_id
                join temp_intersection_link e on c.linkid=e.link_id
                where a.cond_id is not null
                    '''
        self.pg.execute2(sqlcmd_reg)
        count=self.pg.fetchall2()[0][0]
        if count>0:
            sqlcmd_1='''
                select distinct a.cond_id from regulation_relation_tbl a
                join regulation_item_tbl b
                on a.regulation_id=b.regulation_id and b.seq_num=1
                join regulation_item_tbl c
                on a.regulation_id=c.regulation_id and c.seq_num=3
                join temp_intersection_link d on b.linkid=d.link_id
                join temp_intersection_link e on c.linkid=e.link_id
                where a.cond_id is not null
                        '''
            self.pg.execute2(sqlcmd_1)
            self.all_time=self.pg.fetchall2()
            condition_exists_flag=True
        else:
            condition_exists_flag=False  
        return condition_exists_flag
    
    def _update_link_tbl(self):
        
        self.log.info('Begin update link table for dummy link...')
        
        self.CreateTable2('link_tbl_bak_dummy')
        sqlcmd='''
            delete from link_tbl a
            using temp_dummy_link_todelete b
            where a.link_id=b.link_id
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('End update link table for dummy link...')

    def _update_node_tbl(self):
        
        self.log.info('Begin update node table for dummy link...')
        
        self.CreateTable2('temp_dummy_node_todelete')
        sqlcmd = """
                drop table if exists temp_dummy_node_todelete;
                create table temp_dummy_node_todelete
                as
                select  node_id, 
                        array_to_string(array_agg(link_id),'|') as node_lid 
                from
                (
                    select a.node_id, b.link_id
                    from
                    (
                        select s_node as node_id from mid_dummy_link
                        union
                        select e_node as node_id from mid_dummy_link
                    ) as a
                    left join link_tbl as b
                    on a.node_id in (b.s_node,b.e_node)
                ) a 
                group by node_id;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateTable2('node_tbl_bak_dummy')
        sqlcmd='''
            delete from node_tbl a
            using temp_dummy_node_todelete b
            where a.node_id=b.node_id and b.node_lid ='';
            
            update node_tbl a
            set node_lid=b.node_lid
            from temp_dummy_node_todelete b
            where a.node_id=b.node_id and b.node_lid<>'';
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.log.info('End update node table for dummy link...')
        
    def _update_regulation_tbl(self):

        self.log.info('Begin update regulation table for dummy link...')
        
        self.CreateTable2('regulation_item_tbl_bak_dummy')
        self.CreateTable2('regulation_relation_tbl_bak_dummy')
        sqlcmd='''
            delete from regulation_item_tbl a
            using temp_regulation_id b
            where a.regulation_id=b.regulation_id;
            
            delete from regulation_relation_tbl a
            using temp_regulation_id b
            where a.regulation_id=b.regulation_id;
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
               

        sqlcmd = '''
                select max(reg_id) from temp_dummy_reg
                '''
        self.pg.execute2(sqlcmd)
        max_reg_id=self.pg.fetchall2()[0][0]
        sqlcmd = '''
                select max(regulation_id) from regulation_relation_tbl
                '''
        self.pg.execute2(sqlcmd)
        cur_reg_id=self.pg.fetchall2()[0][0]
        for i in range(max_reg_id):
            sqlcmd='''
                select * from temp_dummy_reg 
                where reg_id=%d
                order by seq
                    '''%(i+1)
            self.pg.execute2(sqlcmd)
            results=self.pg.fetchall2()
            sqlcmd='''
                select a.node_id from node_tbl a
                join link_tbl b
                on a.node_id in (b.s_node,b.e_node)
                join link_tbl c
                on a.node_id in (c.s_node,c.e_node)
                where b.link_id=%d and c.link_id=%d
                    '''%(results[0][1],results[1][1])
            self.pg.execute2(sqlcmd)
            node_id=self.pg.fetchall2()[0][0]

            cond_id=results[0][3]
            #print cur_reg_id+i+1,node_id,results[0][-1],results[-1][-1],cond_id
            if cond_id:
                sqlcmd='''
                    insert into regulation_relation_tbl(regulation_id,nodeid,inlinkid,outlinkid,condtype,cond_id)
                    values(%d,%d,%d,%d,42,%d)
                        '''%(cur_reg_id+i+1,node_id,results[0][1],results[-1][1],cond_id)
                self.pg.execute2(sqlcmd)
            else:
                sqlcmd='''
                    insert into regulation_relation_tbl(regulation_id,nodeid,inlinkid,outlinkid,condtype)
                    values(%d,%d,%d,%d,42)
                        '''%(cur_reg_id+i+1,node_id,results[0][1],results[-1][1])
                self.pg.execute2(sqlcmd)
                
            sqlcmd_insertlink='''
                insert into regulation_item_tbl(regulation_id,linkid,seq_num)
                values(%d,%d,%d)
                                '''
            self.pg.execute2(sqlcmd_insertlink%(cur_reg_id+i+1,results[0][1],1))
            sqlcmd_insertnode='''
                insert into regulation_item_tbl(regulation_id,nodeid,seq_num)
                values(%d,%d,%d)
                                '''
            self.pg.execute2(sqlcmd_insertnode%(cur_reg_id+i+1,node_id,2))
            for j in range(len(results)-1):
                self.pg.execute2(sqlcmd_insertlink%(cur_reg_id+i+1,results[j+1][1],j+3))
            self.pg.commit2()
            
        self.log.info('End update regulation table for dummy link.')
    

    def _update_guideinfo_tbl(self):
        
        self.log.info('Begin update guide table for dummy link...')
        
        table_list=['signpost_uc_tbl','spotguide_tbl','lane_tbl','caution_tbl','force_guide_tbl','hook_turn_tbl','signpost_tbl','towardname_tbl']
        
        for table_name in table_list:
            sqlcmd='''
                drop table if exists temp_update_[table_name];
                create table temp_update_[table_name]
                as
                (
                    select distinct d.* from temp_dummy_link_todelete a 
                    join (
                    select gid,1 as ipo,inlinkid as link_id from [table_name]
                    union 
                    select gid,2,outlinkid as link_id from [table_name]
                    union
                    select gid,3,unnest(string_to_array(passlid,'|'))::bigint as link_id from [table_name]
                    ) b using(link_id)
                    join [table_name] d using(gid)
                    order by gid
                )
                    '''
            sqlcmd=sqlcmd.replace('[table_name]',table_name)
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            
            sqlcmd='''
                drop table if exists temp_update_[table_name]_passlid;
                create table temp_update_[table_name]_passlid
                as
                (
                    select gid,
                           replace(mid_findpasslinkbybothnodes(nodeid,tonodeid), ',', '|') as passlid
                    from
                    (
                        select a.gid,nodeid,case when b.s_node in (c.s_node,c.e_node) then b.s_node else b.e_node end as tonodeid
                        from
                        (
                            select gid,nodeid,(passlid_arr[array_upper(passlid_arr,1)])::bigint as lastlinkid,outlinkid
                            from
                            (
                                select gid,nodeid,string_to_array(passlid,'|') as passlid_arr,outlinkid from temp_update_[table_name]
                            ) a
                        ) a
                        join link_tbl b on a.lastlinkid=b.link_id
                        join link_tbl c on a.outlinkid=c.link_id
                    ) a
                )
                    '''
            sqlcmd=sqlcmd.replace('[table_name]',table_name)
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            
            sqlcmd='''
                update [table_name] a
                set passlid=b.passlid,
                passlink_cnt=case when b.passlid is not null then array_upper(string_to_array(b.passlid,'|'),1) else 0 end
                from temp_update_[table_name]_passlid b
                where a.gid=b.gid
                    '''
            sqlcmd=sqlcmd.replace('[table_name]',table_name)
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            
        self.log.info('End update guide table for dummy link...')