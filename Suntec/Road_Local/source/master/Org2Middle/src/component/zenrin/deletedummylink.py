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
        self.CreateTable2('temp_intersection_link')
        
    def _search(self,rec_org,rec,path):
        rec_list=[]
        path=eval(path)
        results=[]
        for link in self.intersection_link:
            if link[0] not in self.dummy_link:
                if link[3] in (1,2) and link[1]==rec[2]:
                    results.append([link[0],link[1],link[2],(link[4]==4)])
                elif link[3] in (1,3) and link[2]==rec[2]:
                    results.append([link[0],link[2],link[1],(link[4]==4)])
                    
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
        results=[]
        for link in self.intersection_link:
            if link[3] in (1,2) and link[1]==rec[2]:
                if self.dict_reg.has_key(rec[0]):
                    for j,cond_id in self.dict_reg[rec[0]]:
                        if j==link[0] and not cond_id:
                            break
                    else:
                        results.append([link[0],link[1],link[2],(link[4]==4)])
                else:
                    results.append([link[0],link[1],link[2],(link[4]==4)])
            elif link[3] in (1,3) and link[2]==rec[2]:
                if self.dict_reg.has_key(rec[0]):
                    for j,cond_id in self.dict_reg[rec[0]]:
                        if j==link[0] and not cond_id:
                            break
                    else:
                        results.append([link[0],link[2],link[1],(link[4]==4)])
                else:
                    results.append([link[0],link[2],link[1],(link[4]==4)])
        
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
        results=[]
        for link in self.intersection_link:
            if link[3] in (1,2) and link[1]==rec[2]:
                if self.dict_reg.has_key(rec[0]):
                    for j,cond_id in self.dict_reg[rec[0]]:
                        if j==link[0] and ( not cond_id or cond_id==time ):
                            break
                    else:  
                        results.append([link[0],link[1],link[2],(link[4]==4)])
                else:
                    results.append([link[0],link[1],link[2],(link[4]==4)])
            elif link[3] in (1,3) and link[2]==rec[2]:
                if self.dict_reg.has_key(rec[0]):
                    for j,cond_id in self.dict_reg[rec[0]]:
                        if j==link[0] and ( not cond_id or cond_id==time ):
                            break
                    else:
                        results.append([link[0],link[2],link[1],(link[4]==4)])
                else:
                    results.append([link[0],link[2],link[1],(link[4]==4)])
        
        for result in results:
            if result[0] in path:
                continue
            
            if not result[3]:
                rec_list.append([result[0],str(path+[result[0]])])

            else:
                rec_list.extend(self._search_reg_spe(rec_org,result,str(path+[result[0]]),time))
        return rec_list
    
    def _Do(self):
        
        self._prepare()
        
        self._calulatenewreg()
        self._update_link_tbl()
        self._update_node_tbl()
        self._update_regulation_tbl()
        self._update_guideinfo_tbl()
        
    def _prepare(self):
        
        self.CreateIndex2('regulation_relation_tbl_regulation_id_idx')
        self.CreateIndex2('regulation_relation_tbl_inlinkid_idx')
        self.CreateIndex2('regulation_relation_tbl_outlinkid_idx')
        self.CreateIndex2('regulation_relation_tbl_cond_id_idx')
        self.CreateIndex2('regulation_relation_tbl_condtype_idx')
        self.CreateIndex2('regulation_item_tbl_regulation_id_idx')
        self.CreateIndex2('regulation_item_tbl_linkid_idx')
        self.CreateIndex2('temp_link_walked_link_id_idx')
        
        self.CreateFunction2('mid_find_intersectionlink')
        
        sqlcmd='''
            select a.linkid,b.linkid,c.cond_id from regulation_item_tbl a 
            join regulation_item_tbl b using(regulation_id)
            join regulation_relation_tbl c using(regulation_id)
            where a.seq_num=1 and b.seq_num=3
                '''
        self.pg.execute2(sqlcmd)
        results=self.pg.fetchall2()
        self.dict_reg={}
        for i,j,cond_id in results:
            if self.dict_reg.has_key(i):
                self.dict_reg[i].append([j,cond_id])
            else:
                self.dict_reg[i]=[[j,cond_id]]
        sqlcmd='''
            select link_id from mid_dummy_link
            order by link_id
                '''
        self.pg.execute2(sqlcmd)
        results=self.pg.fetchall2()
        self.dummy_link=map(lambda x:x[0],results)
        
        self.all_regulation=[]
        
        self.inlinkoutlink={}
        
    def _calulatenewreg(self):
    
        self.log.info('Begin calulate new regulation ...')
        
        
        
        sqlcmd = '''
                select link_id from mid_dummy_link order by link_id
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
                #print 'all_link',self.intersection_link_id
                self.allstartlink=self._searchallstartlink()
                #print self.allstartlink
                condition_exists_flag=self._find_timerelated_regulation()
                while True:
                    delete_link_flag=True
                    self.reg_list=[]
                    for link in self.allstartlink:
                        path=str([link[0]])
                        
                        search_result_a=self._search(link,link,path)
                        result_a=map(lambda x:x[0],search_result_a)
                        self.inlinkoutlink[link[0]]=[set(result_a)]
                        
                        search_result_b=self._search_reg(link,link,path)
                        result_b=map(lambda x:x[0],search_result_b)
                        self.inlinkoutlink[link[0]].append(set(result_b))
                        self.inlinkoutlink[link[0]].append({})
                        #print result_a
                        #print search_result_a
                        #print result_b
                        #print search_result_b
                        if set(result_a)<>set(result_b):
                            
                            set_a=set(result_a)-set(result_b)
                            #print set_a
                            for i in set_a:
                                for j in search_result_a:
                                    if j[0]==i:
                                        self.reg_list.append([eval(j[1]),None])
                            if len(set(result_b)-set(result_a))>0:
                                delete_link_flag=False
                                set_b=set(result_b)-set(result_a)
                                for i in set_b:
                                    for j in search_result_b:
                                        if j[0]==i:
                                            for link_id in eval(j[1]):
                                                if link_id in self.dummy_link:
                                                    self.dummy_link.remove(link_id)
                                                sqlcmd='''delete from mid_dummy_link where link_id=%d'''
                                                self.pg.execute(sqlcmd%link_id)
                                self.pg.commit2()
                        if condition_exists_flag:
                            
                            for time in self.all_time:
                                result_c=map(lambda x:x[0],self._search_reg_spe(link,link,path,time))
                                if set(result_b)<>set(result_c):
                                    set_a=set(result_b)-set(result_c)
                                    self.inlinkoutlink[link[0]][2][time]=set(result_c)
                                    for i in set_a:
                                        for j in search_result_a:
                                            if j[0]==i:
                                                self.reg_list.append([eval(j[1]),time])                       
                    #print delete_link_flag
                    if delete_link_flag:
                        self._insertnewregulation()
                        break

                    
        self.log.info('End calulate new regulation ...')    


    def _searchintersectionlink(self,i):
        
        sqlcmd='''
                select mid_find_intersectionlink(%d)
                '''%i[0]
                
        self.pg.execute2(sqlcmd)
        
        sqlcmd='''
                select link_id,s_node,e_node,one_way_code,link_type 
                from temp_intersection_link
                order by link_id
                '''
        self.pg.execute2(sqlcmd)
        
        self.intersection_link=self.pg.fetchall2()
        self.intersection_link_id=map(lambda x:x[0],self.intersection_link)
        
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
                order by a.link_id
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
        
        self.all_regulation.extend(self.reg_list)
           
    def _find_timerelated_regulation(self):
        
        self.all_time=set([])
        condition_exists_flag=False
        for i in self.intersection_link_id:
            if self.dict_reg.has_key(i):
                for j,cond_id in self.dict_reg[i]:
                    if j in self.intersection_link_id and cond_id:
                        self.all_time.add(cond_id)
                        condition_exists_flag=True
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
               
        max_reg_id=len(self.all_regulation)
        sqlcmd = '''
                select max(regulation_id) from regulation_relation_tbl
                '''
        self.pg.execute2(sqlcmd)
        cur_reg_id=self.pg.fetchall2()[0][0]
        for i in range(max_reg_id):
            #print i
            sqlcmd='''
                select a.node_id from node_tbl a
                join link_tbl b
                on a.node_id in (b.s_node,b.e_node)
                join link_tbl c
                on a.node_id in (c.s_node,c.e_node)
                where b.link_id=%d and c.link_id=%d
                    '''%(self.all_regulation[i][0][0],self.all_regulation[i][0][1])
            self.pg.execute2(sqlcmd)
            node_id=self.pg.fetchall2()[0][0]

            cond_id=self.all_regulation[i][1]
            #print cur_reg_id+i+1,node_id,results[0][-1],results[-1][-1],cond_id
            if cond_id:
                sqlcmd='''
                    insert into regulation_relation_tbl(regulation_id,nodeid,inlinkid,outlinkid,condtype,cond_id)
                    values(%d,%d,%d,%d,1,%d)
                        '''%(cur_reg_id+i+1,node_id,self.all_regulation[i][0][0],self.all_regulation[i][0][-1],cond_id)
                self.pg.execute2(sqlcmd)
            else:
                sqlcmd='''
                    insert into regulation_relation_tbl(regulation_id,nodeid,inlinkid,outlinkid,condtype)
                    values(%d,%d,%d,%d,1)
                        '''%(cur_reg_id+i+1,node_id,self.all_regulation[i][0][0],self.all_regulation[i][0][-1])
                self.pg.execute2(sqlcmd)
                
            sqlcmd_insertlink='''
                insert into regulation_item_tbl(regulation_id,linkid,seq_num)
                values(%d,%d,%d)
                                '''
            self.pg.execute2(sqlcmd_insertlink%(cur_reg_id+i+1,self.all_regulation[i][0][0],1))
            sqlcmd_insertnode='''
                insert into regulation_item_tbl(regulation_id,nodeid,seq_num)
                values(%d,%d,%d)
                                '''
            self.pg.execute2(sqlcmd_insertnode%(cur_reg_id+i+1,node_id,2))
            for j in range(len(self.all_regulation[i][0])-1):
                self.pg.execute2(sqlcmd_insertlink%(cur_reg_id+i+1,self.all_regulation[i][0][j+1],j+3))
            self.pg.commit2()
            
        self.log.info('End update regulation table for dummy link.')
    
    def _update_guideinfo_tbl(self):
        
        self.log.info('Begin update guide table for dummy link...')
        
        table_list=['signpost_uc_tbl','spotguide_tbl','lane_tbl','caution_tbl','force_guide_tbl','hook_turn_tbl','signpost_tbl','towardname_tbl']
        
        for table_name in table_list:
            sqlcmd='''
                drop table if exists [table_name]_bak_dummy;
                create table [table_name]_bak_dummy
                as
                (
                    select * from [table_name]
                );
                
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
                        join link_tbl_bak_dummy b on a.lastlinkid=b.link_id
                        join link_tbl_bak_dummy c on a.outlinkid=c.link_id
                    ) a
                )
                    '''
            sqlcmd=sqlcmd.replace('[table_name]',table_name)
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            
            sqlcmd='''
                update [table_name] a
                set passlid=b.passlid,
                passlink_cnt=case when b.passlid is not null and b.passlid<>'' 
                                    then array_upper(string_to_array(b.passlid,'|'),1) else 0 end
                from temp_update_[table_name]_passlid b
                where a.gid=b.gid
                    '''
            sqlcmd=sqlcmd.replace('[table_name]',table_name)
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            
        self.log.info('End update guide table for dummy link...')
    
    def _DoCheckLogic(self):
        
        sqlcmd='''
            select inlinkid,outlinkid,cond_id from regulation_relation_tbl
            where outlinkid is not null
                '''
        self.pg.execute2(sqlcmd)
        results=self.pg.fetchall2()
        self.dict_reg={}
        for i,j,cond_id in results:
            if self.dict_reg.has_key(i):
                self.dict_reg[i].append([j,cond_id])
            else:
                self.dict_reg[i]=[[j,cond_id]]        
        for inlinkid in self.inlinkoutlink:
            result_a=self.inlinkoutlink[inlinkid][0]
            result_b=self.inlinkoutlink[inlinkid][1]
            result_c=self.inlinkoutlink[inlinkid][2]
            if not self.dict_reg.has_key(inlinkid):
                if not result_a==result_b:
                    return False
                for time in result_c:
                    if not result_a==result_c[time]:
                        return False
            else:
                for j,cond_id in self.dict_reg[inlinkid]:
                    if not cond_id:
                        result_a=result_a-set([j])
                if result_a<>result_b:
                    return False
                for time in result_c:
                    temp_result_b=result_b
                    for j,cond_id in self.dict_reg[inlinkid]:
                        if cond_id==time:
                            temp_result_b=temp_result_b-set([j])
                    if temp_result_b<>result_c[time]:
                        return False
        return True