#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""Graph utilities."""
from io import open
from six.moves import range, zip, zip_longest
from six import iterkeys
from collections import defaultdict, Iterable
import random

import numpy as np
import pandas as pd
import time




class Graph(defaultdict):

    def __init__(self,nodes_number):
        self.w_matrix = np.zeros(shape=(nodes_number, nodes_number))
        super().__init__(list)  # 这是python3.x的语法

    def nodes(self):
        return self.keys()

    def adjacency_iter(self):
        """
        dict_items([(1, [2, 4, 5, 6]), (2, [1, 3]),...,)
        返回节点和它的邻居
        """
        return self.items()  # 源代码这里写的是iteritems()

    def subgraph(self, nodes={}):
        """
        这个函数返回的是graph的子图
        :param nodes:
        :return:
        """
        subgraph = Graph()

        for n in nodes:
            if n in self:
                subgraph[n] = [x for x in self[n] if x in nodes]

        return subgraph

    def make_undirected(self):
        """
        将有向图变为无向图
        :return:
        """

        # t0 = time()

        for v in list(self.keys()):
            # print("pre self.keys() len: ",len(self.keys()))
            for other in self[v]:
                if v != other:
                    # print("v,other",v,other)
                    # print(other in self)
                    """
                          考虑这样一种情形：
                          1:[2,3...]
                          2:[4,...]
                          当遍历1号节点时，发现2在1的list中，于是执行self[2].append(1)，这样变成了：
                          1:[2,3...]
                          2:[4,...,1]
                          现在遍历到了2号节点，发现1在2的list中，于是执行self[1].append(2),这样变成了：
                          1:[2,3...,2] 出现了两个2
                          2:[4,...,1]
                          """
                    # if v != other and v not in self[other]:
                    # #这行代码可以替代上一行，不需要下面的去重操作，但是需要排序
                    self[other].append(v)
                    # print(other,v,len(self))
                    # print(other,v)
            # print("post self.keys() len: ", len(self.keys()))


        # t1 = time()
        # logger.info('make_directed: added missing edges {}s'.format(t1 - t0))

        self.make_consistent()
        """这个函数的作用是：
        1. 用来去除重复的，因为之前的工作会得到酱紫的输出：
            {1: [2, 5, 6, 2, 4, 5, 6], 2: [1, 3], ...}，每一对节点都保留了两条边
        2. 同时，这个函数还可以保证每一项的list是从小到达排序的。
        3. 最后这个函数内部调用了remove_self_loops()，可以去除掉自循环（自己到自己的边）
        """
        return self

    def make_consistent(self):
        """这个函数的作用是：
        1. 用来去除重复的，因为之前的工作会得到酱紫的输出：
            {1: [2, 5, 6, 2, 4, 5, 6], 2: [1, 3], ...}，每一对节点都保留了两条边
        2. 同时，这个函数还可以保证每一项的list是从小到达排序的。
        3. 最后这个函数内部调用了remove_self_loops()，可以去除掉自循环（自己到自己的边）
        """
        # t0 = time()
        for k in iterkeys(self):
            self[k] = list(sorted(set(self[k])))
            # 原来的每一项list self[k]先进行set函数去重，
            # 然后经过sorted函数进行排序，
            # 最后转化为list进行重新赋值
        # t1 = time()
        # logger.info('make_consistent: made consistent in {}s'.format(t1 - t0))

        # print("not remove_self_loops", self)
        self.remove_self_loops()
        # print("remove_self_loops:",self)

        return self

    def remove_self_loops(self):
        """
        去除掉节点到自己的自循环
        :return:
        """
        # removed = 0
        # t0 = time()

        for x in self:
            if x in self[x]:
                self[x].remove(x)
                # removed += 1

        # t1 = time()
        #
        # logger.info('remove_self_loops: removed {} loops in {}s'.format(removed, (t1 - t0)))
        return self

    def check_self_loops(self):
        """
        检查是否含有自回路
        :return:
        """
        for x in self:
            for y in self[x]:
                if x == y:
                    return True

        return False

    def has_edge(self, v1, v2):
        """
        判断两个节点之间是否有边
        :param v1:
        :param v2:
        :return:
        """
        if v2 in self[v1] or v1 in self[v2]:
            return True
        return False

    def degree(self, nodes=None):
        """
        这个函数用来求节点的度数对于有向图而言，求的是入度或初度（具体是哪个要看你到dic是根据什么构建的）
        输入的nodes参数可以是一个节点编号，也可以是一系列节点的可迭代对象如list。如果是单个节点，则直接返回
        节点的度，如果是一些列节点，则返回dic，key对应为目标节点，value为该节点对应的度数。
        :param nodes:
        :return:
        """
        if isinstance(nodes, Iterable):
            return {v: len(self[v]) for v in nodes}
        else:
            return len(self[nodes])



    def number_of_edges(self):
        "Returns the number of nodes in the graph"
        """
        请注意，这里的边的个数对应的是无向图，如果是有向图，需要去掉除以2的操作
        """
        return sum([self.degree(x) for x in self.keys()]) / 2

    def number_of_nodes(self):
        "Returns the number of nodes in the graph"
        return len(self)  # 这个是我们删掉了order旧接口后的代码


    def random_walk(self, path_length, alpha=0, rand=random.Random(), start=None,r = 0.1):
        """ Returns a truncated random walk.
            返回一些较短的随机游走路径
            path_length: Length of the random walk.
            alpha: probability of restarts.
            start: the start node of the random walk.
            random.Random()：产生0-1之间的随机浮点数
            请注意：这里的随机游走路径未必是连续的，有可能是走着走着突然回到起点接着走
        """
        G = self
        if start:
            path = [start]
        else:
            # Sampling is uniform w.r.t V, and not w.r.t E
            path = [rand.choice(list(G.keys()))]

        while len(path) < path_length:
            cur = path[-1]
            if len(G[cur]) > 0:  # 当cur有邻居时
                if rand.random() >= alpha:  # 这是一个经典的概率编程的语句
                    """
                    这条语句成立的概率是1-alpha
                    """

                    samples=random.sample(G[cur], int(r * len(self[cur])))
                    if len(samples)>0:
                        path.append(rand.choice(samples))
                    else:
                        path.append(start)
                    """
                    上面这段代码是为了使deepwalk与我们ada_random_walk在同一个起跑线上做比较，因为我们的ada_random_walk
                    在这里用了这样的采样（尽管我们也可以不才样，但是运算时间会更大，采样后，30个进程，需要运行30分钟，
                    不才样可能会运行更长）
                    """

                    # path.append(rand.choice(G[cur]))
                else:
                    path.append(path[0])  # 这里应该写的是path.append(cur)吧？
                    """
                    以概率1-alpha从当前节点继续向前走，或者以alpha的概率restart
                    """
            else:
                break
        return [str(node) for node in path]


    def ada_random_walk_wth_pq(self, path_length, rand=random.Random(), start=None, importance_content=None,r=0.1,p=0.5,q=0.5):
        """
              这个函数是我自己写的
              Returns a truncated random walk.
                  返回一些较短的随机游走路径
                  path_length: Length of the random walk.
                  alpha: probability of restarts.
                  start: the start node of the random walk.
                  random.Random()：产生0-1之间的随机浮点数
                  请注意：这里的随机游走路径未必是连续的，有可能是走着走着突然回到起点接着走
              """

        if start:
            path = [start]
        else:
            # Sampling is uniform w.r.t V, and not w.r.t E
            path = [rand.choice(list(self.keys()))]
        cur = path[-1]
        if len(self[cur]) == 0:
            path = path * path_length
            return [str(node) for node in path]
        else:
            path.append(rand.choice(self[cur]))
        """
        上面这个语句不安全，因为self[cur]有可能为[]
        """
        cc_back_up = importance_content

        while len(path) < path_length:
            prior = path[-2]
            cur = path[-1]
            p_cur_x = defaultdict(float)

            bias = 0.1
            for x in random.sample(self[cur], int(r * len(self[cur]))):
                # for x in self[cur]:
                if x in self[prior]:
                    # p_cur_x[x]=self.degree(nodes=x)*(cc_back_up[prior])
                    p_cur_x[x] = cc_back_up[x] * cc_back_up[prior] + bias
                    """
                    计算clustering_coefficient也是一个瓶颈，实际上这个根本不需要再一次计算，因为上面的
                    代码已经计算好了，这里用cc_back_up[prior]即可
                    这里使用self.degree(nodes=x)来代替w_curr,x是不科学的，但只是对无权图的使用，
                    如果边是带权重的，我们希望这里是边的权重。
                    """
                elif x == prior:
                    p_cur_x[x] = 1.0 / p * cc_back_up[x] * cc_back_up[prior] + bias
                else:
                    # p_cur_x[x]=self.degree(nodes=x)*(1-cc_back_up[prior])
                    p_cur_x[x] = 1.0 / q * cc_back_up[x] * (1 - cc_back_up[prior]) + bias
                    # print("x not in self[prior]!, p_cur_x= ", p_cur_x[x])
                    """
                      这里使用self.degree(nodes=x)来代替w_curr,x是不科学的，但只是对无权图的使用，
                      如果边是带权重的，我们希望这里是边的权重。
                      cc,bc,取值范围都是0-1，所以 (1 - cc_back_up[prior])是合理的
                      degree，page rank取值范围不是0-1,所以 (1 - cc_back_up[prior])不合理，你需要在外面传入degree的时候，进行规格化

                    """

            p_cur_x_normalization = defaultdict(float)
            sum = 0
            for key in p_cur_x.keys():
                # print("key: ",key," p_cur_x[key]: ",p_cur_x[key])
                sum = sum + p_cur_x[key]
            # print("sum: ",sum)
            for key in p_cur_x.keys():
                p_cur_x_normalization[key] = p_cur_x[key] * 1.0 / sum

            sum = 0
            p_cur_x_step = defaultdict(float)
            for key in p_cur_x_normalization.keys():
                sum = sum + p_cur_x_normalization[key]
                p_cur_x_step[key] = sum
            # print("p_cur_x_step  done!")
            seletor = rand.random()
            # print("seletor: ",seletor)
            # print("p_cur_x_step: ",p_cur_x_step)
            n = 0
            # print("start ",start)
            for key in p_cur_x_step.keys():
                if seletor <= p_cur_x_step[key]:
                    n = key
                    # print("n=,",n)
                    break

            path.append(n)

        return [str(node) for node in path]




    def node2vec_walk(self, path_length, rand=random.Random(), start=None, p=0.25,
                               q=0.25,r = 0.1):
        """
                这个函数是我自己写的
                Returns a truncated random walk.
                    返回一些较短的随机游走路径
                    path_length: Length of the random walk.
                    alpha: probability of restarts.
                    start: the start node of the random walk.
                    random.Random()：产生0-1之间的随机浮点数
                    请注意：这里的随机游走路径未必是连续的，有可能是走着走着突然回到起点接着走
                """

        if start:
            path = [start]
        else:
            # Sampling is uniform w.r.t V, and not w.r.t E
            path = [rand.choice(list(self.keys()))]
        cur = path[-1]
        if len(self[cur]) == 0:
            path = path * path_length
            return [str(node) for node in path]
        else:
            path.append(rand.choice(self[cur]))
        """
        上面这个语句不安全，因为self[cur]有可能为[]
        """

        while len(path) < path_length:
            prior = path[-2]
            cur = path[-1]
            p_cur_x = defaultdict(float)


            for x in random.sample(self[cur], int(r * len(self[cur]))):
                # for x in self[cur]:
                if x in self[prior]:
                    # p_cur_x[x]=self.degree(nodes=x)*(cc_back_up[prior])
                    p_cur_x[x] = 1
                    """
                    计算clustering_coefficient也是一个瓶颈，实际上这个根本不需要再一次计算，因为上面的
                    代码已经计算好了，这里用cc_back_up[prior]即可
                    这里使用self.degree(nodes=x)来代替w_curr,x是不科学的，但只是对无权图的使用，
                    如果边是带权重的，我们希望这里是边的权重。
                    """
                elif x == prior:
                    p_cur_x[x] = 1.0 / p
                else:
                    # p_cur_x[x]=self.degree(nodes=x)*(1-cc_back_up[prior])
                    p_cur_x[x] = 1.0 / q
                    # print("x not in self[prior]!, p_cur_x= ", p_cur_x[x])
                    """
                      这里使用self.degree(nodes=x)来代替w_curr,x是不科学的，但只是对无权图的使用，
                      如果边是带权重的，我们希望这里是边的权重。
                    """

            p_cur_x_normalization = defaultdict(float)
            sum = 0
            for key in p_cur_x.keys():
                sum = sum + p_cur_x[key]

            for key in p_cur_x.keys():
                p_cur_x_normalization[key] = p_cur_x[key] * 1.0 / sum

            sum = 0
            p_cur_x_step = defaultdict(float)
            for key in p_cur_x_normalization.keys():
                sum = sum + p_cur_x_normalization[key]
                p_cur_x_step[key] = sum
            # print("p_cur_x_step  done!")
            seletor = rand.random()
            # print("seletor: ",seletor)
            # print("p_cur_x_step: ",p_cur_x_step)
            n = 0
            # print("start ",start)
            for key in p_cur_x_step.keys():
                if seletor <= p_cur_x_step[key]:
                    n = key
                    # print("n=,",n)
                    break

            path.append(n)

        return [str(node) for node in path]




def build_deepwalk_corpus(G, num_paths, path_length, alpha=0,
                          rand=random.Random(0)):
    """
    这个函数可以对一个图生成一个语料库
    :param G:
    :param num_paths:
    :param path_length:
    :param alpha:
    :param rand:
    :return:
    """
    walks = []

    nodes = list(G.nodes())

    for cnt in range(num_paths):
        # 这条语句将下面的random walk过程重复了num_paths次，
        # 相当于每一个节点做num_paths次random walk
        rand.shuffle(nodes)
        for node in nodes:  # 这条语句对图所有的节点各自进行一次random walk
            walks.append(G.random_walk(path_length, rand=rand, alpha=alpha, start=node))

    return walks


def build_deepwalk_corpus_iter(G, num_paths, path_length, alpha=0,
                               rand=random.Random(0),importance_content=None,walk_type="ada_walk",r=0.1,p=0.1,q=0.1):
    """
    这个函数可以迭代地生成语料库
    :param G:
    :param num_paths:
    :param path_length:
    :param alpha:
    :param rand:
    :return:
    """
    """
    'ada_walk', 'deep_walk','node2vec_walk'
    """

    nodes = list(G.nodes())

    for cnt in range(num_paths):
        rand.shuffle(nodes)
        count=0
        for node in nodes:
            count = count+1
            if walk_type=='deep_walk':
                yield G.random_walk(path_length, rand=rand, alpha=alpha, start=node,r=r) #,importance_content=importance_content)
            # elif walk_type=='ada_walk':
            #     yield G.ada_random_walk(path_length, rand=rand, start=node,importance_content=importance_content,r=r)
            elif walk_type=='node2vec_walk':
                yield G.node2vec_walk(path_length, start=node,p=p,q=q,r=r)
            elif walk_type=='ada_random_walk_wth_pq':
                yield G.ada_random_walk_wth_pq(path_length, rand=rand, start=node,
                                               importance_content=importance_content,r=r,p=p,q=q)



# http://stackoverflow.com/questions/312443/how-do-you-split-a-list-into-evenly-sized-chunks-in-python
def grouper(n, iterable, padvalue=None):
    "grouper(3, 'abcdefg', 'x') --> ('a','b','c'), ('d','e','f'), ('g','x','x')"
    return zip_longest(*[iter(iterable)] * n, fillvalue=padvalue)

def load_edgelist(file_, undirected=True,weighted=True,nodes_number=0,sep=" ", thresh=0.05):
    G = Graph(nodes_number=nodes_number)
    # w_matrix=np.zeros(shape=(nodes_number,nodes_number))
    for i in range(0,nodes_number):
        G[i]
    with open(file_) as f:
        for l in f:
            x, y, w = l.strip().split(sep=sep)
            x = int(x)
            y = int(y)
            w = float(w)

            if weighted:
                G.w_matrix[x,y]=w
                G[x].append(y)
                time.sleep(1000)
            else:
                # print("process this graph as unweighted graph")
                if w > thresh:
                    G.w_matrix[x, y] = 1
                    G[x].append(y)



            if undirected:
                if weighted:
                    G.w_matrix[y,x] = w
                else:
                    if w > thresh:
                        G.w_matrix[x, y] = 1
                        G[y].append(x)


    # import matplotlib.pyplot as plt
    # import seaborn as sns
    #
    # fig,ax=plt.subplots(1,2)
    # sns.set(color_codes=True)
    # aa=G.w_matrix.reshape((1,-1))[0]
    # sns.distplot(aa,ax=ax[0])
    #
    # G.w_matrix=(G.w_matrix-G.w_matrix.min())/(G.w_matrix.max()-G.w_matrix.min())
    #
    # # G.w_matrix = (G.w_matrix - G.w_matrix.mean()) / G.w_matrix.std()
    #
    # # G.w_matrix = (G.w_matrix) / G.w_matrix.sum()
    #
    # # G.w_matrix = 1.0/(1+np.exp(-G.w_matrix))
    #
    # # G.w_matrix = (G.w_matrix) / G.w_matrix.max()
    #
    # #
    # # print(G.w_matrix)
    # # print(G.w_matrix.max(),G.w_matrix.min())
    #
    # bb=G.w_matrix.reshape((1,-1))[0]
    # sns.distplot(bb, ax=ax[1])
    # plt.savefig("./edge_weight_distribution_tfidf_sampled_as_unweighted.jpg")
    # plt.show()



    G.make_consistent()


    return(G)


def load_graph(graph,label,undirected=True,weighted=True,thresh=0.05):

    node_number = pd.read_csv(label, header=None, sep=" ").shape[0]
    graph = load_edgelist(graph, undirected=undirected, weighted=weighted, nodes_number=node_number,thresh=thresh)
    return graph


if __name__ == "__main__":
    graph_base="../../data/raw/mypersonality_final/mypersonality"
    graph=load_graph(graph_base)

    # print(graph.w_matrix)
