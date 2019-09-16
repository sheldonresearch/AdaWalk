#!usr/bin/env python  
# -*- coding:utf-8 _*-  
""" 
@project:deepwalk-master
@author:xiangguosun 
@contact:sunxiangguodut@qq.com
@website:http://blog.csdn.net/github_36326955
@file: simple_main.py
@platform: macOS High Sierra 10.13.1 Pycharm pro 2017.1 
@time: 2018/09/13 
"""

import sys
import random

from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter
from src.personality_regression import graph
from src.personality_regression import simple_walks as serialized_walks
from gensim.models import Word2Vec
import time
import networkx as nx
from collections import defaultdict
import pandas as pd


def process(args):
    print(args)

    G=graph.load_graph(graph=args.graph,label=args.label,undirected=args.undirected,weighted=args.weighted ,thresh=args.thresh)
    print("Number of nodes: {}".format(G.number_of_nodes()))
    print("Number of edges: {}".format(G.number_of_edges()))
    print("number_walks: {}".format(args.number_walks))
    num_walks = G.number_of_nodes() * args.number_walks
    print("Number of walks: {}".format(num_walks))
    data_size = num_walks * args.walk_length
    print("Data size (walks*length): {}".format(data_size))

    # time.sleep(1000)

    if args.walk_type in ["ada_random_walk_wth_pq"]:
        if args.importance == "cc" and not args.weighted: #这个针对的是没有权重的图，对于完全图和带权重的图，需要计算带权重的聚类系数
            print("We now pre-comput clustering_coefficient...")
            Gnx = nx.Graph(data=G)
            importance_content_dic = nx.clustering(Gnx)
            importance_content = defaultdict(float, importance_content_dic)
        elif args.importance == "cc" and args.weighted: #这个针对的是没有权重的图，对于完全图和带权重的图，需要计算带权重的聚类系数
            print("We now pre-comput weighted clustering_coefficient...")
            Gnx=nx.from_pandas_adjacency(pd.DataFrame(G.w_matrix))

            importance_content_dic = nx.clustering(Gnx,weight="weight")
            importance_content=defaultdict(float,importance_content_dic)
        # elif args.importance == "pr":
        #     print("We now pre-comput page_rank...")
        #     Gnx = nx.Graph(data=G)
        #     importance_content = defaultdict(float)
        #     importance_content_non_norm = defaultdict(float)
        #     importance_content_dic = nx.pagerank(Gnx)
        #     sum = []
        #     for key, value in importance_content_dic.items():
        #         sum.append(value)
        #         importance_content_non_norm[key] = value
        #     for key, value in importance_content_dic.items():
        #         importance_content[key] = 1.0 * importance_content_non_norm[key] / max(sum)
        elif args.importance == "de":
            print("We now pre-comput degree...")
            importance_content = defaultdict(float)
            importance_content_non_norm = defaultdict(float)
            importance_content_dic = G.degree(nodes=G.keys())
            sum = []
            for key, value in importance_content_dic.items():
                sum.append(value)
                importance_content_non_norm[key] = value
            for key, value in importance_content_dic.items():
                importance_content[key] = 1.0 * importance_content_non_norm[key] / max(sum)
        #
        # elif args.importance == "bc":
        #     print("We now pre-comput betweenness_centrality...")
        #     Gnx = nx.Graph(data=G)
        #     importance_content = defaultdict(float)
        #     importance_content_dic = nx.betweenness_centrality(Gnx)
        #     for key, value in importance_content_dic.items():
        #         importance_content[key] = value
        else:
            raise Exception(
                "Unknown importance type: '%s'.  Valid formats: 'clustering_coefficient', 'page_rank','degree'" % args.importance)
    else:
        importance_content = None


    t0 = time.time()
    print("Walking... @ ",time.asctime())

    walks_filebase =args.walks_out +  "walks.txt"
    walk_files = serialized_walks.write_walks_to_disk(G, walks_filebase, num_paths=args.number_walks,
                                                      path_length=args.walk_length, alpha=0,
                                                      rand=random.Random(args.seed),
                                                      num_workers=args.workers,importance_content=importance_content,
                                                      walk_type=args.walk_type,r=args.r,p=args.p,q=args.q)
    walks = serialized_walks.WalksCorpus(walk_files)
    t1=time.time()
    print("walking finished! using ",time.asctime())
    t_delta=t1-t0
    m,s=divmod(t_delta,60)
    h,m=divmod(m,60)
    print(" time consuming: %d:%02d:%02d." %(h,m,s))

    print("Training...")
    model = Word2Vec(walks, size=args.representation_size, window=args.window_size, min_count=0, sg=1, hs=1,
                     workers=args.workers)

    model.wv.save_word2vec_format(args.output)


def main():
    parser = ArgumentParser("AdaWalk_for_personality_project",
                            formatter_class=ArgumentDefaultsHelpFormatter,
                            conflict_handler='resolve')
    parser.add_argument('--importance', default='cc',
                        choices=['cc',
                                 'pr',
                                 'de',
                                 'bc'],
                        help='importance type')

    parser.add_argument('--walk_type', default='ada_random_walk_wth_pq',
                        choices=['deep_walk',
                                 'node2vec_walk',
                                 'ada_random_walk_wth_pq'],
                        help='random walk type')


    parser.add_argument('--number-walks', default=200, type=int,
                        help='Number of random walks to start at each node')

    parser.add_argument('--output', required=True,
                        help='Output representation file')

    parser.add_argument('--graph', required=True,
                        help='graph')
    parser.add_argument('--label', required=True,
                        help='label')

    parser.add_argument('--walks_out', required=True,
                        help='Output representation file')
    parser.add_argument('--representation-size', default=64, type=int,
                        help='Number of latent dimensions to learn for each node.')

    parser.add_argument('--seed', default=0, type=int,
                        help='Seed for random walk generator.')

    parser.add_argument('--undirected', action='store_true',
                        help='Treat graph as undirected.')

    parser.add_argument('--walk-length', default=1000, type=int,
                        help='Length of the random walk started at each node')

    parser.add_argument('--window-size', default=4, type=int,
                        help='Window size of skipgram model.')

    parser.add_argument('--workers', default=1, type=int,
                        help='Number of parallel processes.')
    parser.add_argument('--r', default=0.1, type=float,
                        help='sample ratio for neighbors.')
    parser.add_argument('--thresh', default=0.05, type=float,
                        help='sample ratio for graph.')
    parser.add_argument('--p', default=1.0, type=float,
                        help='p.')
    parser.add_argument('--q', default=1.0, type=float,
                        help='q.')
    parser.add_argument('--weighted', action='store_true',
                        help='Treat graph as weighted graph.')

    """for deepwalk and node2vec"""

    importance_list = ['de']
    p_list = ['0.1']
    q_list = ['0.1']
    number_walks_list = ['5']
    walk_length_list = ['10']
    representation_size_list = ['128']
    window_size_list = ['3']
    r_list = ['0.1']
    thresh_list = ['0.08']


    data_name="SoCE"
    walks_out = "../../results/walks/"
    graph = "../../results/graphs/"+data_name+"_edgelist_tfidf.txt"
    label = "../../data/regular_data/"+data_name+"_labels.txt"

    for importance in importance_list:
        for p in p_list:
            for q in q_list:
                for nw in number_walks_list:
                    for wl in walk_length_list:
                        for rs in representation_size_list:
                            for ws in window_size_list:
                                for r in r_list:
                                    for thresh in thresh_list:
                                        parameters = '_' + importance + '_' + p + '_' + q + '_' + nw + '_' + wl + '_' + rs + '_' + ws + '_' + r + '_' + thresh
                                        out_path = "../../results/features/"+data_name+"_feature_tfidf" + parameters + "_adawalktxt"
                                        args = parser.parse_args(("--graph " + graph +
                                                                  " --label " + label +
                                                                  " --walk_type ada_random_walk_wth_pq "
                                                                  " --number-walks " + nw + " "
                                                                " --importance " + importance + " "
                                                                " --walk-length " + wl + " "
                                                                " --representation-size " + rs + " "
                                                                " --window-size " + ws + " "
                                                                " --workers 4"
                                                                " --undirected"
                                                                  # " --weighted False "
                                                                 " --r " + r + " "
                                                                  " --thresh " + thresh + " "
                                                                  " --p " + p + " "
                                                                  " --q " + q + " "
                                                                  " --walks_out " + walks_out +
                                                                  " --output " + out_path).split())

                                        process(args)
                                        print("done!")












    #
    #
    # """for adawalk:"""
    # importance_list=['de','cc']
    # p_list=['0.1','0.5','2']
    # q_list = ['0.1', '0.5', '2']
    # number_walks_list=['5','10','20']
    # walk_length_list=['3','5','10']
    # representation_size_list=['64','128','200']
    # window_size_list=['3','5','10']
    # r_list=['0.1','0.2','0.5','0.9']
    # thresh_list=['0.1','0.2','0.3','0.4','0.5','0.6']
    #
    # data_name = "PAN"
    # walks_out = "../../results/walks/"
    # graph = "../../results/graphs/" + data_name + "_edgelist_mairesse.txt"
    # label = "../../data/regular_data/" + data_name + "_labels.txt"
    #
    #
    #
    # for importance in importance_list:
    #     for p in p_list:
    #         for q in q_list:
    #             for nw in number_walks_list:
    #                 for wl in walk_length_list:
    #                     for rs in representation_size_list:
    #                         for ws in window_size_list:
    #                             for r in r_list:
    #                                 for thresh in thresh_list:
    #                                     parameters='_'+importance+'_'+p+'_'+q+'_'+nw+'_'+wl+'_'+rs+'_'+ws+'_'+r+'_'+thresh
    #                                     out_path="/usr/PycharmProjects/AdaWalk_backup"+"/PAN_mairesse_features/"+data_name+"_feature_adawalk"+parameters+".txt"
    #                                     args = parser.parse_args(("--graph " + graph +
    #                                                               " --label " + label +
    #                                                                      " --walk_type ada_random_walk_wth_pq "
    #                                                                      " --number-walks "+nw+" "
    #                                                                      " --importance "+importance+" "
    #                                                                      " --walk-length "+wl+" "
    #                                                                      " --representation-size "+rs+" "
    #                                                                      " --window-size "+ws+" "
    #                                                                      " --workers 4"
    #                                                                      " --undirected"
    #                                                                      # " --weighted False "
    #                                                                      " --r "+r+" "
    #                                                                      " --thresh "+thresh+" "
    #                                                                      " --p "+p+" "
    #                                                                      " --q "+q+" "
    #                                                                      " --walks_out "+walks_out+
    #                                                                      " --output "+out_path).split())
    #
    #                                     process(args)
    #                                     print("done!")





if __name__ == "__main__":

    sys.exit(main())
