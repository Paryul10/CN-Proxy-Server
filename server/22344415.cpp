#include<bits/stdc++.h>
using namespace std;

int n,m;

int isvalid(int x,int y)
{
    if(x>=0 && x<n && y>=0 && y<m)
    {
        return 1;
    }
    return 0;
}

int main()
{
    int t;
    cin >> t;
    while(t--)
    {
        cin >> n >> m;
        int i,j,k;

        int arr[n][m];

        for(i=0;i<n;i++)
        {
            for(j=0;j<m;j++)
            {
                arr[i][j]=0;
            }
        }

        int vis[101];

        // for(i=1;i<101;i++)
        // {
        //     vis[]
        // }

        int max=1;

        for(i=0;i<n;i++)
        {
            for(j=0;j<m;j++)
            {
                int val1=-1,val2=-1,val3=-1,val4=-1;
                if(isvalid(i,j-2))
                {
                    val1=arr[i][j-2];
                }    

                if(isvalid(i-1,j-1))
                {
                    val2=arr[i-1][j-1];
                }

                if(isvalid(i-1,j+1))
                {
                    val3=arr[i-1][j+1];
                }

                if(isvalid(i-2,j))
                {
                    val4=arr[i-2][j];
                }

                for(k=1;k<=max;k++)
                {
                    if(k!=val1 && k!=val2 && k!=val3 && k!=val4)
                    {
                        arr[i][j]=k;
                        break;
                    }
                }

                if(arr[i][j]==0)
                {
                    arr[i][j]=max+1;
                    max++;
                }
            }
        }


        cout << max << '\n';
        for(i=0;i<n;i++)
        {
            for(j=0;j<m;j++)
            {
                cout << arr[i][j] << " ";
            }
            cout << '\n';
        }


    }
}
