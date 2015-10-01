#coding:utf8
'''
Created on 2015年8月13日

@author: zhengchao
'''
import copy
class search_road():
    
    def __init__(self,pg,link_id,dir=0,onewayflg=False,thrucondition=None,endcondition=None,depth=None,table_name='link_tbl',s_node_name='s_node',e_node_name='e_node',onewaydict=None,onewayname=None,errorcondition=None):
        
        self.dir=dir
        self.onewayflg=onewayflg
        self.start_link_id=link_id
        self.tc=thrucondition if thrucondition else 'true'
        self.ec=endcondition
        self.errc=errorcondition
        self.depth=depth        
        self.pg=pg
        if self.start_link_id:
            self.init_path()
    
    def init_path(self):
        self.pg.execute('''
                    select s_node,e_node,one_way_code from link_tbl 
                    where link_id = %s
                        '''%self.start_link_id)

        self.connectcondition={1:'''and  ((s_node = %d and one_way_code in (1,2))
                                or
                                (e_node = %d and one_way_code in (1,3)))''',
                               2:''' and ((s_node = %d and one_way_code in (1,3))
                                or
                                (e_node = %d and one_way_code in (1,2)))'''}
        a=self.pg.fetchall2()[0]
        #print a
        self.paths=[]
        if a[2]<>4:
            if a[2]==1:
                if self.dir==0:
                    self.paths=[{'path':[self.start_link_id],'node_id':a[0],'endflag':None,'dir':1},{'path':[self.start_link_id],'node_id':a[0],'endflag':None,'dir':2},{'path':[self.start_link_id],'node_id':a[1],'endflag':None,'dir':1},{'path':[self.start_link_id],'node_id':a[1],'endflag':None,'dir':2}]
                if self.dir==1:
                    self.paths=[{'path':[self.start_link_id],'node_id':a[0],'endflag':None,'dir':1},{'path':[self.start_link_id],'node_id':a[0],'endflag':None,'dir':2}]
                if self.dir==2:
                    self.paths=[{'path':[self.start_link_id],'node_id':a[1],'endflag':None,'dir':1},{'path':[self.start_link_id],'node_id':a[1],'endflag':None,'dir':2}]
            if a[2]==2:
                if self.dir==0:
                    self.paths=[{'path':[self.start_link_id],'node_id':a[0],'endflag':None,'dir':2},{'path':[self.start_link_id],'node_id':a[1],'endflag':None,'dir':1}]
                if self.dir==1:
                    self.paths=[{'path':[self.start_link_id],'node_id':a[0],'endflag':None,'dir':2}]
                if self.dir==2:
                    self.paths=[{'path':[self.start_link_id],'node_id':a[1],'endflag':None,'dir':1}]
            if a[2]==3:
                if self.dir==0:
                    self.paths=[{'path':[self.start_link_id],'node_id':a[0],'endflag':None,'dir':1},{'path':[self.start_link_id],'node_id':a[1],'endflag':None,'dir':2}]
                if self.dir==1:
                    self.paths=[{'path':[self.start_link_id],'node_id':a[0],'endflag':None,'dir':1}]
                if self.dir==2:
                    self.paths=[{'path':[self.start_link_id],'node_id':a[1],'endflag':None,'dir':2}]
        #print 'p',self.paths,'d',a[2],self.dir
    def is_thr_path(self,link_id):
        if not self.tc:
            return True
        sqlcmd='''
            select * from link_tbl
            where link_id = %d and %s
                '''
        self.pg.execute(sqlcmd%(link_id,self.tc))
        #print sqlcmd%(link_id,self.tc)
        results=self.pg.fetchall2()
        if len(results)==1:
            return True
        return False
        
        
    def is_end_path(self,link_id):
        
        if not self.ec:
            return False
        sqlcmd='''
                select * from link_tbl 
                where link_id = %d and %s
                '''
        self.pg.execute(sqlcmd%(link_id,self.ec))
        results=self.pg.fetchall2()
        if len(results)==1:
            return True
        return False
    
    def is_error_path(self,link_id):
        
        if not self.errc:
            return False
        sqlcmd='''
                select * from link_tbl 
                where link_id = %d and %s
                '''
        #print sqlcmd%(link_id,self.errc)
        self.pg.execute(sqlcmd%(link_id,self.errc))
        results=self.pg.fetchall2()
        if len(results)==1:
            return True
        return False
        
    def search_one_level(self):
        
        search_flag=False
        for path in self.paths:
            if path['endflag']:
                continue
            
            search_flag=True
            sqlcmd='''
                select link_id,case when s_node=%d then e_node else s_node end 
                from link_tbl
                where link_id not in (%s) %s
                   '''
            if self.onewayflg:
                #print path
                connectcondition=self.connectcondition[path['dir']]%(path['node_id'],path['node_id'])
                
            else:
                connectcondition=''
            #print sqlcmd%(path['node_id'],','.join(list(str(i) for i in path['path'])),self.tc,connectcondition)
            self.pg.execute(sqlcmd%(path['node_id'],','.join(list(str(i) for i in path['path'])),connectcondition))
            
            
            results=self.pg.fetchall2()
            if len(results)==0:
                path['endflag']=1
                continue
            tmp_path=copy.deepcopy(path)
            path['path'].append(results[0][0])
            path['node_id']=results[0][1]
            if not self.is_thr_path(path['path'][-1]):
                path['endflag']=1
            if self.is_end_path(path['path'][-1]):
                path['endflag']=2
            if self.is_error_path(path['path'][-1]):
                path['endflag']=3
            for result in results[1:]:
                newpath=copy.deepcopy(tmp_path)
                newpath['path'].append(result[0])
                newpath['node_id']=result[1]
                if not self.is_thr_path(newpath['path'][-1]):
                    newpath['endflag']=1
                if self.is_end_path(newpath['path'][-1]):
                    newpath['endflag']=2
                self.paths.append(newpath)
            #print self.paths
        return search_flag
        
    def deal_with_sth(self):
        
        pass
    
    def search(self):
        
        if self.paths:
            if self.depth:
                for i in range(self.depth):
                    self.search_one_level()
            else:
                while self.search_one_level():
                    pass
            
    def return_paths(self):
        #print self.paths
        return self.paths

if __name__=='__main__':
    ramp=search_road(link_id=3920441080653,dir=2,onewayflg=True,endcondition=' link_type<>5')
    ramp.search()
    ramp.return_paths()