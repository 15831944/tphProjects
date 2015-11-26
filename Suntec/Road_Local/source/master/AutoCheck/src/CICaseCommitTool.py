#! /usr/bin/python
import argparse
import json
import os
import urllib,urllib2

from TAP.tap_processor import TAP_Processor

def _do(project_id,plan_name,json_path,tap_path):
    case_id_result = get_case_id(project_id,plan_name,json_path)
    commit_test_result(project_id,tap_path,case_id_result)
    
def get_case_id(project_id,plan_name,json_path):
    json_dict = json.load(file(json_path))
    build_name = "_".join(("auto",json_dict["Proj"],
                           json_dict["Area"],json_dict["Country"]))
    build_descp = json_dict["Route_DBName"]
    
    send_dict={}
    send_dict['project_id'] = project_id
    send_dict['planname'] = plan_name
    send_dict['buildname'] = build_name
    send_dict['builddescp'] = build_descp
    send_result_data = json.dumps(send_dict)
    
    url = 'http://192.168.8.137:5000/create_new_execution/'
    
    try:
        a = urllib2.urlopen(url, urllib.urlencode({"data":send_result_data}))
        return a.readlines()[0]
    except urllib2.HTTPError, e:
        print e.code
        

def commit_test_result(project_id,tap_path , case_id_result):
    url =  'http://192.168.8.137:5000/create_results_for_execution/'
    case_id_dict = json.loads(case_id_result)
    build_id = case_id_dict["build_id"]
    case_list = case_id_dict["case_list"]
    case_dict = {}
    for item in case_list:
        for k in item:
            case_dict[item[k].encode("utf-8")] = k
    
    tp = TAP_Processor()
  
    file_names = os.listdir(tap_path)
    for fileName in file_names:
        if fileName.split(".")[-1] == "tap":
            tp.loadTap(os.path.join(tap_path,fileName))
    
    test_result_dict = tp.getAllResultDict()
    
    result_list = []
    for (k,v) in test_result_dict.items():
        if(case_dict.has_key(str(k))):
            result_list.append(case_dict[k]+":"+v)
          
    send_dict = {}
    send_dict['project_id'] = project_id
    send_dict['buildid'] = build_id
    send_dict["resultlist"] = result_list
    send_result_data = json.dumps(send_dict)
    

    try:
        urllib2.urlopen(url, urllib.urlencode({"data":send_result_data}))
    except urllib2.HTTPError, e:
        print e.code
    

def make_parse(parser): 
    parser.add_argument('project_id', help= 'project_id') 
    parser.add_argument('plan_name', help= 'plan_name') 
    parser.add_argument('json_path', help= 'json_path') 
    parser.add_argument('tap_path', help= 'tap_path') 
    return parser
 
 
def main(args):
    _do(args.project_id, args.plan_name,args.json_path, args.tap_path)
  
      
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    make_parse(parser)
    main(parser.parse_args())


