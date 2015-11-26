import component.component_base

class deletedummylink_zenrin(component.component_base.comp_base):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Link')

    def _DoCreateTable(self):
        
        self.CreateTable2('temp_link_dummy_walked')
        self.CreateTable2('mid_dummy_reg')
        self.CreateTable2('mid_dummy_todelete')
        self.CreateTable2('org_notin_gid')
        
    def _search(self,rec_org,rec,path):
        rec_list=[]
        path=eval(path)
        sqlcmd='''
                select meshcode,linkno,case when oneway=0 then case when snodeno=%d then snodeno else enodeno end else snodeno end as snodeno,
                case when oneway=0 then case when snodeno=%d then enodeno else snodeno end else enodeno end as enodeno,elcode 
                from temp_dummy_link
                where meshcode='%s' and (oneway=0 and %d in (snodeno,enodeno)
                or (oneway=1 and snodeno=%d)) and substr(elcode,4,1)<>'8'
                '''
        self.pg.execute2(sqlcmd%(rec[3],rec[3],rec[0],rec[3],rec[3]))
        results=self.pg.fetchall2()
        for result in results:
            if [result[0],result[1]] in path:
                continue
            
            if result[4][3] not in ('8','6'):
                rec_list.append([int(result[0])*10000000+result[1],str(path+[[result[0],result[1]]])])
            else:
                rec_list.extend(self._search(rec_org,result,str(path+[[result[0],result[1]]])))
        return rec_list
    
    def _search_reg(self,rec_org,rec,path):
        rec_list=[]
        #path_list=[]
        path=eval(path)
        #print path
        sqlcmd='''
                select a.meshcode,a.linkno,case when a.oneway=0 then case when a.snodeno=%d then a.snodeno else a.enodeno end else a.snodeno end as snodeno,
                case when oneway=0 then case when a.snodeno=%d then a.enodeno else a.snodeno end else a.enodeno end as enodeno,elcode 
                from temp_dummy_link a
                left join "org_not-in" b
                on a.meshcode=b.meshcode and %d=fromlinkno and b.tolinkno=a.linkno
                where a.meshcode='%s' and (oneway=0 and %d in (a.snodeno,a.enodeno)
                or (oneway=1 and a.snodeno=%d)) and b.tolinkno is null
                '''
        self.pg.execute2(sqlcmd%(rec[3],rec[3],rec[1],rec[0],rec[3],rec[3]))
        results=self.pg.fetchall2()
        for result in results:
            if [result[0],result[1]] in path:
                continue
            
            if result[4][3] not in ('8','6'):
                rec_list.append([int(result[0])*10000000+result[1],str(path+[[result[0],result[1]]])])

            else:
                rec_list.extend(self._search_reg(rec_org,result,str(path+[[result[0],result[1]]])))
        return rec_list
    
    def _Do(self):

        sqlcmd = '''
                select meshcode,linkno from org_road where substr(elcode,4,1)='8'
                '''      
        
        self.pg.execute2(sqlcmd)
        alldummylink=self.pg.fetchall2()
        #print a
        
        sqlcmd_2='''
                select count(1) from temp_link_dummy_walked
                where meshcode='%s' and linkno=%d
                    '''
        sqlcmd_insert='''
                insert into temp_link_dummy_walked
                (meshcode,linkno)
                values('%s',%d)
                        '''
        reg_id=0
        for i in alldummylink:
            self.pg.execute2(sqlcmd_2%(i[0],i[1]))
            cnt=self.pg.fetchall2()[0][0]
            if cnt==0:
                dummylink_arr=[i]
                self.pg.execute2(sqlcmd_insert%(i[0],i[1]))
                self.pg.commit2()
                while True:
                    cnt=0
                    for j in dummylink_arr:
                        
                        sqlcmd_3='''
                        select b.meshcode,b.linkno from org_road a 
                        join org_road b
                        on st_intersects(a.the_geom,b.the_geom) and substr(b.elcode,4,1)='8'
                        left join temp_link_dummy_walked c
                        on b.meshcode=c.meshcode and b.linkno=c.linkno
                        where a.meshcode='%s' and a.linkno=%d and c.linkno is null
                                '''
                        self.pg.execute2(sqlcmd_3%(j[0],j[1]))
                        results=self.pg.fetchall2()
                        cnt+=len(results)
                        
                        for result in results:
                            dummylink_arr.append(result)
                            self.pg.execute2(sqlcmd_insert%(result[0],result[1]))
                            self.pg.commit2()
                    if cnt==0:
                        break
                print 'all_link:',dummylink_arr
                self.CreateTable2('temp_dummy_link')
                sqlcmd_insertdummy='''
                        insert into temp_dummy_link
                        (meshcode,linkno,snodeno,enodeno,oneway,elcode,the_geom)
                        select meshcode,linkno,snodeno,enodeno,oneway,elcode,the_geom
                        from org_road
                        where meshcode='%s' and linkno=%d
                                    '''
                for i in dummylink_arr:
                    self.pg.execute2(sqlcmd_insertdummy%(i[0],i[1]))
                    self.pg.commit2()
                #exit()
                self.CreateTable2('temp_dummy_node')
                #exit()
                sqlcmd_alllinknear='''
                        insert into temp_dummy_link
                        (meshcode,linkno,snodeno,enodeno,oneway,elcode,the_geom)
                        select distinct a.meshcode,a.linkno,a.snodeno,a.enodeno,a.oneway,a.elcode,a.the_geom from org_road a
                        join 
                        temp_dummy_link b
                        on st_intersects(a.the_geom,b.the_geom)
                        left join temp_dummy_link c
                        on a.meshcode=c.meshcode and a.linkno=c.linkno
                        where c.linkno is null
                        '''
                self.pg.execute2(sqlcmd_alllinknear)
                self.pg.commit2()
                sqlcmd_reg='''
                        select count(1) from "org_not-in" a 
                        join temp_dummy_link b on a.meshcode=b.meshcode and a.fromlinkno=b.linkno
                        join temp_dummy_link c on a.meshcode=c.meshcode and a.tolinkno=c.linkno
                        where day<>'100000000' or shour<>'0000' or ehour<>'2400'
                            '''
                self.pg.execute2(sqlcmd_reg)
                count=self.pg.fetchall2()[0][0]
                if count>0:
                    continue
                sqlcmd_startlink='''
                        select a.meshcode,linkno,case when oneway=0 then (case when b.nodeno is not null then snodeno else enodeno end) else snodeno end as snodeno,
                            case when oneway=0 then (case when b.nodeno is not null then enodeno else snodeno end) else enodeno end as enodeno,elcode 
                            from temp_dummy_link a
                        left join temp_dummy_node b
                        on a.enodeno=b.nodeno
                        where substr(a.elcode,4,1) not in ('8','6') and 
                                (oneway=0 or 
                                    (oneway=1 and b.nodeno is not null))
                                '''
                self.pg.execute2(sqlcmd_startlink)
                allstartlink=self.pg.fetchall2()
                flag1=True
                flag2=True
                reg_list=[]
                for link in allstartlink:
                    path=str([[link[0],link[1]]])
                    
                    dict_a=dict(self._search(link,link,path))
                    result_a=map(lambda x:x[0],self._search(link,link,path))
                    
                    
                    path=str([[link[0],link[1]]])
                    
                    result_b=map(lambda x:x[0],self._search_reg(link,link,path))
                    
                    if set(result_a)<>set(result_b):
                        print '-'*100
                        print link[:2], result_a
                        print link[:2], result_b
                        print set(result_a)-set(result_b)
                        
                        
                        set_a=set(result_a)-set(result_b)
                        for i in set_a:
                            print 'new reg:',dict_a[i]
                            reg_list.append(dict_a[i])
                        flag1=False
                        if len(set(result_b)-set(result_a))>0:
                            print 'sssss'
                            #exit()
                            flag2=False
                if flag1:
                    print 'no reg'
                if not flag1 and flag2:
                    
                    sqlcmd_3='''
                            insert into org_notin_gid(gid)
                            select gid from "org_not-in" a 
                            join temp_dummy_link b on a.meshcode=b.meshcode and a.fromlinkno=b.linkno
                            join temp_dummy_link c on a.meshcode=c.meshcode and a.tolinkno=c.linkno;
                            
                            insert into mid_dummy_todelete(meshcode,linkno)
                            select meshcode,linkno from temp_dummy_link
                            where substr(elcode,4,1)='8';
                                                '''
                    self.pg.execute2(sqlcmd_3)
                    self.pg.commit2()
                    for reg in reg_list:
                        reg_id+=1
                        reg1=eval(reg)
                        reg1=map(lambda x:int(x[0])*10000000+x[1],reg1)
                        sqlcmd_insertreg='''
                            insert into mid_dummy_reg(reg_id,link_id,seq)
                            values(%d,%d,%d);
                                            '''
                        
                        #print 'reg',reg1
                        for idx,link_id in enumerate(reg1):
                            self.pg.execute2(sqlcmd_insertreg%(reg_id,link_id,idx+1))
                self.pg.commit2()