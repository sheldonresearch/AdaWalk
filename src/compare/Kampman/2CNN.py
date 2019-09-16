#!/usr/bin/env python
# encoding: utf-8

"""
@version: python3.6
@author: Xiangguo Sun
@contact: sunxiangguo@seu.edu.cn
@site: http://blog.csdn.net/github_36326955
@software: PyCharm
@file: 2CLSTM.py
@time: 17-7-27 5:15pm
"""

"""
this is a implement of Kampman et al. Investigating audio, video, and text fusion methods for 
end to end automatic personality prediction, ACL, pages 606-611, 2018

"""

import os
import numpy as np
import pandas as pd
from keras.models import  Model
from keras.layers.embeddings import Embedding
from keras.layers import Dense,Reshape, Flatten,Conv2D, Concatenate, Input,MaxPooling2D
import time

from keras.preprocessing.text import Tokenizer
from keras.preprocessing.sequence import pad_sequences
from sklearn.model_selection import train_test_split
from keras.callbacks import EarlyStopping
from keras.callbacks import ModelCheckpoint
from keras.callbacks import ReduceLROnPlateau




import tensorflow as tf
import keras.backend.tensorflow_backend as KTF

#进行配置，使用70%的GPU
os.environ["CUDA_VISIBLE_DEVICES"]="0"
config = tf.ConfigProto()
config.gpu_options.per_process_gpu_memory_fraction = 0.5
session = tf.Session(config=config)

# 设置session
KTF.set_session(session )


GLOVE_DIR="./glove.twitter.27B.100d.txt"



def get_embeddings():
    embeddings_index = {}
    with open(GLOVE_DIR) as f:
        for line in f:
            values = line.split()
            word = values[0]
            coefs = np.asarray(values[1:], dtype='float32')
            embeddings_index[word] = coefs
    print('Found %s word vectors.' % len(embeddings_index))
    return embeddings_index



input_length = 100
MAX_NB_WORDS = 5000
w2vDimension = 100
# VALIDATION_SPLIT = 0.2

embeddings_index = get_embeddings()




data_name="youtube"
feature_to_path="../../../results/features/"+data_name+"_"

read_texts="../../../data/regular_data/"+data_name+"_text.txt"
read_labels="../../../data/regular_data/"+data_name+"_labels.txt"


texts_pd=pd.read_table(read_texts,header=None,sep=" ")
texts_pd.columns=["node_id","node_text"]

labels_pd=pd.read_table(read_labels,header=None,sep=" ")


labels_pd.columns=["node_id","trait1","trait2","trait3","trait4","trait5"]

mer=pd.merge(texts_pd,labels_pd,left_on='node_id',right_on='node_id')


def regression(train_x, train_label, text_x, text_label):
    clf = MultiOutputRegressor(svm.SVR(gamma='scale'))

    clf.fit(train_x, train_label)
    y_pred = pd.DataFrame(clf.predict(text_x))

    catagory = y_pred.shape[1]
    # Person=np.corrcoef(text_label.iloc[:,],y_pred,rowvar=False)
    # print(text_label.iloc[:,0])
    # print(text_label.shape)
    # print("Person: ")
    # print(Person.shape)
    RMSE = np.sqrt(mean_squared_error(text_label, y_pred, multioutput='raw_values'))

    result = []
    for i in range(0, catagory):
        result.append(RMSE[i])
    return result


train_ratio=0.5
results=[]
for time_e in range(0,10):
    print("test: ", time_e," /",10,"...")
    training,testing =train_test_split(mer,test_size=int(train_ratio*mer.shape[0]),shuffle=True)


    texts_train, labels_train = training[['node_text']],training[["trait1","trait2","trait3","trait4","trait5"]]
    texts_test, labels_test=testing[['node_text']],testing[["trait1","trait2","trait3","trait4","trait5"]]

    texts_train=np.reshape(texts_train.as_matrix(),newshape=(-1,))
    labels_train=labels_train.as_matrix()
    texts_test=np.reshape(texts_test.as_matrix(),newshape=(-1,))
    labels_test=labels_test.as_matrix()


    print(texts_train.shape,labels_train.shape,texts_test.shape,labels_test.shape)




    # print(texts_train)

    tokenizer = Tokenizer(MAX_NB_WORDS)
    tokenizer.fit_on_texts(texts_train)
    sequences_train = tokenizer.texts_to_sequences(texts_train)
    sequences_test = tokenizer.texts_to_sequences(texts_test)
    word_index = tokenizer.word_index
    data_train = pad_sequences(sequences_train, maxlen=input_length)
    data_test = pad_sequences(sequences_test, maxlen=input_length)


    classes = 5

    n_symbols = min(MAX_NB_WORDS, len(word_index))+1
    embedding_weights = np.zeros((n_symbols, w2vDimension))
    for word, i in word_index.items():
        if i >= MAX_NB_WORDS:
            continue
        embedding_vector = embeddings_index.get(word)
        if embedding_vector is not None:
            embedding_weights[i] = embedding_vector


    #start our model
    hidden_dim_1 = 100
    hidden_dim_2 = 100

    input_a = Input(shape=(input_length,))

    embedding_layer=Embedding(output_dim=w2vDimension,
                            input_dim=n_symbols,
                            mask_zero=False,
                            weights=[embedding_weights],
                            input_length=input_length,
                            trainable=False)(input_a)
    print("embedding_layer.shape: ",embedding_layer.shape)
    embedding_layer_reshape=Reshape(target_shape=(input_length,w2vDimension,1))(embedding_layer)


    conv1=Conv2D(filters=64,kernel_size=(3,w2vDimension),activation="relu")(embedding_layer_reshape)
    conv2=Conv2D(filters=64,kernel_size=(4,w2vDimension),activation="relu")(embedding_layer_reshape)
    conv3=Conv2D(filters=64,kernel_size=(5,w2vDimension),activation="relu")(embedding_layer_reshape)




    max1 = MaxPooling2D(pool_size=(input_length-3+1, 1))(conv1)
    max2 = MaxPooling2D(pool_size=(input_length-4+1, 1))(conv2)
    max3 = MaxPooling2D(pool_size=(input_length-5+1, 1))(conv3)

    concatenate = Concatenate(axis=3)([max1, max2, max3])


    flat=Flatten()(concatenate)
    dense=Dense(64,activation="relu",name='vectors')(flat)
    output = Dense(classes,activation="softmax")(dense)

    model = Model(input=input_a, output=output)
    # model end


    #compile the model
    # from sklearn import metrics
    from keras import losses
    # model.compile(loss=losses.mean_squared_error, optimizer='sgd', metrics=[metrics.accuracy_score])
    model.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])
    print(model.summary())


    #
    # from keras.models import load_model
    # try:
    #     print("load pre_trained weigt...")
    #     model = load_model('./logs/2CNN_weight_not_augment_'+class_type+'.hdf5')
    #     print("pre_trained weight loaded!")
    # except:
    #     print("OOPs: pre_trained weight loaded failed!!")


    # # callbacks:
    # tb = TensorBoard(log_dir='../logs/compare',  # log 目录
    #                  histogram_freq=1,  # 按照何等频率（epoch）来计算直方图，0为不计算
    #                  batch_size=64,     # 用多大量的数据计算直方图
    #                  write_graph=True,  # 是否存储网络结构图
    #                  write_grads=False, # 是否可视化梯度直方图
    #                  write_images=False,# 是否可视化参数
    #                  embeddings_freq=0,
    #                  embeddings_layer_names=None,
    #                  embeddings_metadata=None)

    es=EarlyStopping(monitor='val_loss', patience=20, verbose=0)

    mc=ModelCheckpoint(
        './2CNN.hdf5',
        monitor='val_loss',
        verbose=0,
        save_best_only=True,
        save_weights_only=False,
        mode='auto',
        period=1
    )

    rp=ReduceLROnPlateau(
        monitor='val_loss',
        factor=0.5,
        patience=5,
        verbose=0,
        mode='min',
        epsilon=0.01,
        cooldown=0,
        min_lr=0.01
    )

    # callbacks = [es,tb,rp]
    callbacks = [es,mc,rp]



    print(data_train)


    # start to train out model
    bs = 32
    ne = 50
    hist = model.fit(data_train, labels_train,batch_size=bs,epochs=ne,
                          verbose=2,validation_split=0.25,callbacks=callbacks)

    print("train process done!!")


    vectors=Model(input=model.input,output=model.get_layer('vectors').output)

    x_train=pd.DataFrame(vectors.predict(data_train))
    y_train=pd.DataFrame(labels_train)
    x_test=pd.DataFrame(vectors.predict(data_test))
    y_test=pd.DataFrame(labels_test)

    print(x_train.shape,y_train.shape,x_test.shape,y_test.shape,labels_test.shape)




    # print("results from the deep learning model")
    # pre=pd.DataFrame(model.predict(data_test))
    # # print("prediction:")
    # # print(pre)
    # # print("actually: ")
    # labels_test=pd.DataFrame(labels_test)
    # # print(labels_test)
    # # MSE=metrics.mean_squared_error(labels_test, pre)
    # # print("MSE:")
    # # print(MSE)
    # from sklearn.metrics import mean_squared_error
    # RMSE=np.sqrt(mean_squared_error(labels_test, pre,multioutput='raw_values'))
    # print('rmse: ')
    # print(RMSE)

    print("====================================================")
    print("results from the vectors:")
    from sklearn.multioutput import MultiOutputRegressor
    from sklearn import svm
    from sklearn.metrics import mean_squared_error



    result = regression(x_train, y_train, x_test, y_test)

    print(result)
    results.append((result))

results=pd.DataFrame(results)

print(results)
print(results.min())
print(results.mean())