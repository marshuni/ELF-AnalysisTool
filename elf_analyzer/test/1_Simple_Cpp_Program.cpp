#include<cstdio>
#include<algorithm>

using namespace std;
void solve()
{
    int n,k;
    scanf("%d%d",&n,&k);
    int b=1;
    for(int i=1,tmp;i<=n;i++)
    {
        scanf("%d",&tmp);
        b*=tmp;
    }
    if(2023%b == 0)
    {
        printf("YES\n");
        int a=2023/b;
        printf("%d ",a);
        for(int i=1;i<k;i++)
            printf("1 ");
        printf("\n");
    }
    else
        printf("NO\n");
    return;
}
int main()
{
    int t=1;
    scanf("%d",&t);
    while(t--)
        solve();
    return 0;
}