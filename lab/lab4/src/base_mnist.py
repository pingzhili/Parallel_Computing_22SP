from matplotlib import pyplot as plt
import numpy as np
from utils import get_mnist_data, init_layer, forward, loss_function, softmax, sigmoid
np.random.seed(42)


X, Y, X_test, Y_test = get_mnist_data()
size = 1
# Split validation
rand=np.arange(60000)
np.random.shuffle(rand)
train_no=rand[:50000]
val_no=np.setdiff1d(rand,train_no)
X_train,X_val=X[train_no,:,:],X[val_no,:,:]
Y_train,Y_val=Y[train_no],Y[val_no]

# Init model
l1 = init_layer(28*28,128)
l2 = init_layer(128,10)

# Training
epochs = 5
lr = 1e-4
batch_size = 32
print(f"Iteration times: {int(epochs*X.shape[0]//(batch_size*size))}")
for i in range(int(epochs*X.shape[0]//(batch_size*size))):
    sample_idx = np.random.randint(0, X_train.shape[0], size = batch_size)
    x=X_train[sample_idx].reshape((-1,28*28))
    y=Y_train[sample_idx]

    output, update_l1, update_l2 = forward(x, y, l1, l2)

    output_category = np.argmax(output, axis=1)
    loss = loss_function(output_category, y)

    l1 -= lr * update_l1
    l2 -= lr * update_l2
    if i % 50 == 0:
        print("Epoch: {}, Loss: {:.4f}".format(int(i/(X.shape[0]//(batch_size*size))), loss.item()))

# plt.ylim(-0.1, 1.1)
# plt.plot(accuracy_scores)
# plt.plot(accuracy_scores_val)
# plt.show()

# test_out=np.argmax(softmax(sigmoid(X_test.dot(l1)).dot(l2)),axis=1)
# test_accuracy=(test_out==Y_test).mean().item()
# print("Test Accuracy: {:.4f}".format(test_accuracy))