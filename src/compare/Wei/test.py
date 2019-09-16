import pandas as pd

le=pd.DataFrame([pd.Series([4,5,6],index=[2,1,3]),pd.Series([7,8,9],index=[2,1,3])]).transpose()
print(le)
ll=le.reset_index(drop=True)
print(ll)