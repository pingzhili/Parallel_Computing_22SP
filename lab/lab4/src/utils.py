import numpy as np
import requests, gzip, os, hashlib

path = '/Users/lipingzhi/Desktop/paral_course/lab/lab4/data'

def fetch(url):
    fp = os.path.join(path, hashlib.md5(url.encode('utf-8')).hexdigest())
    if os.path.isfile(fp):
        with open(fp, "rb") as f:
            data = f.read()
    else:
        with open(fp, "wb") as f:
            data = requests.get(url).content
            f.write(data)
    return np.frombuffer(gzip.decompress(data), dtype=np.uint8).copy()

def get_mnist_data():
    X_train = fetch("http://yann.lecun.com/exdb/mnist/train-images-idx3-ubyte.gz")[0x10:].reshape((-1, 28, 28))
    Y_train = fetch("http://yann.lecun.com/exdb/mnist/train-labels-idx1-ubyte.gz")[8:]
    X_test = fetch("http://yann.lecun.com/exdb/mnist/t10k-images-idx3-ubyte.gz")[0x10:].reshape((-1, 28*28))
    Y_test = fetch("http://yann.lecun.com/exdb/mnist/t10k-labels-idx1-ubyte.gz")[8:]
    return X_train, Y_train, X_test, Y_test

def init_layer(x,y):
    layer=np.random.uniform(-1.,1.,size=(x,y))/np.sqrt(x*y)
    return layer.astype(np.float32)

def sigmoid(x):
    return 1/(1+np.exp(-x))

def sigmoid_deriv(x):
    exp_elem = np.exp(-x)
    return exp_elem/(1+exp_elem)**2

def softmax(x):
    exp_elem = np.exp(x-x.max())
    return exp_elem/np.sum(exp_elem, axis=0)

def softmax_deriv(x):
    exp_elem = np.exp(x-x.max())
    return exp_elem/np.sum(exp_elem, axis=0)*(1-exp_elem/np.sum(exp_elem, axis=0))

def forward(x, y, l1, l2):
    targets = np.zeros((len(y),10), np.float32)
    targets[range(targets.shape[0]),y] = 1

    x_l1=x.dot(l1)
    x_sigmoid=sigmoid(x_l1)
    x_l2=x_sigmoid.dot(l2)
    out=softmax(x_l2)
   
    error=2*(out-targets)/out.shape[0]*softmax_deriv(x_l2)
    update_l2=x_sigmoid.T@error
    
    error=((l2).dot(error.T)).T*sigmoid_deriv(x_l1)
    update_l1=x.T@error

    return out,update_l1,update_l2 

def loss_function(out, y):
    return ((out-y)**2).mean()