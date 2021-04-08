#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MaxTeamNumber 15//�������
#define MaxYearNumber 3//�������
#define MaxCenterClusterNumber 3//ѡȡ�����Ĵ�����
#define MaxNumber 1000.0//��������ɼ�ֵ
#define MinNumber 0.0//�������С�ɼ�ֵ
#define NullFlag 0//��0��ʾ��

typedef double INSTANCE_TYPE;//��ӵ����Ĵصľ�����������
typedef double GRADE_TYPE;//�ض���ɼ�����������  0.1875���������0.1899995��float���µġ�������Ҫ�����double
typedef int TEAMID_TYPE;//��ӵ�ID��������
typedef int CENTERCLUSTERID_TYPE;//���Ĵص�ID��������
/*
 * ��ݽṹ��
 */
typedef struct year {
    GRADE_TYPE grade;
} Year;

/*
 * ���ҽṹ��
 */
typedef struct team {
    TEAMID_TYPE teamId;//��ӵ�ID
    CENTERCLUSTERID_TYPE centerClusterId;//�������Ĵص��״�ID
    Year years[MaxYearNumber];//�������
    INSTANCE_TYPE instances[MaxCenterClusterNumber];//��ӵ����Ĵصľ�������
} Team;

/*
 * ��Ҫ�õ���һЩ���߷���
 */
GRADE_TYPE findMax(GRADE_TYPE *grades);//�����ɼ�
GRADE_TYPE findMin(GRADE_TYPE *grades);//����С�ɼ�
/*
 * ����������飬���й�񻯴���
 */
void standardGrade(Team *teams);

/*
 * �������ֵ����Сֵ�ʹ���񻯵�ֵ
 */
GRADE_TYPE standardData(GRADE_TYPE Max, GRADE_TYPE Min, GRADE_TYPE unStandardData);

/*
 * ��ʼ�����Ĵأ����ݴ�������id���г�ʼ��
 */
void init_CenterCluster(Team *CenterClusters, Team *teams);

/*
 * ��ʼ���ɼ�����
 */
void init_GradeData(Team *teams);

/*
 * ��ʼ���������� 
 */
void init_Instance(Team *CenterClusters, Team *teams);

/*
 * �����������Ҷ��󣬽��о������
 */
GRADE_TYPE caculateInstance(Team team, Team centerCluster);

int findCenterCluster(Team *CenterClusters, Team *teams);

void caculateNewCenterCluster(Team *CenterClusters, Team *teams);

int returnArrayLength(TEAMID_TYPE *ids);

/*
 * �����ҵ���ͬ���teamId
 */
void findSameCluster(Team *teams, TEAMID_TYPE *idArray, CENTERCLUSTERID_TYPE id);

/*
 * ��ʼ���յ�����
 */
void newNullArray(TEAMID_TYPE *idArray);

/*
 * ���໯������1.������룬2.�������Ĵ� 3.���¼������Ĵ�--->���ж�һ���Ƿ�������Ĵأ�1--->2
 */
void k_means(Team *CenterClusters, Team *teams);

void printResult(Team *teams);

void idToTeamName(int id);

void idToClusterName(int id);

int main() {
    Team teams[MaxTeamNumber];
    Team centerClusters[MaxCenterClusterNumber];

    init_GradeData(teams);//��ʼ����ȡ���������
    init_CenterCluster(centerClusters, teams);//��ʼ�����Ĵ�
    k_means(centerClusters,teams);//k-means�㷨
    return 0;
}

GRADE_TYPE findMax(GRADE_TYPE *grades) {
    GRADE_TYPE result = MinNumber;
    for (int i = 0; i < MaxTeamNumber; ++i) {
        if (grades[i] > result) {
            result = grades[i];
        }
    }
    return result;
}

GRADE_TYPE findMin(GRADE_TYPE *grades) {
    GRADE_TYPE result = MaxNumber;
    for (int i = 0; i < MaxTeamNumber; ++i) {
        if (grades[i] < result) {
            result = grades[i];
        }
    }
    return result;
}

GRADE_TYPE standardData(GRADE_TYPE Max, GRADE_TYPE Min, GRADE_TYPE unStandardData) {
    GRADE_TYPE result;
    result = (unStandardData - Min) / (Max - Min);
    //�������뱣����λ����
    result = (int) (100.0 * result + 0.5) / 100.0;
    return result;
}

void standardGrade(Team *teams) {
    GRADE_TYPE grades[MaxYearNumber][MaxTeamNumber];//�������ݳɼ���ά����

    for (int i = 0; i < MaxYearNumber; ++i) {
        for (int j = 0; j < MaxTeamNumber; ++j) {
            grades[i][j] = teams[j].years[i].grade;
        }
    }
    //��񻯲���
    for (int j = 0; j < MaxTeamNumber; ++j) {
        for (int i = 0; i < MaxYearNumber; ++i) {
            GRADE_TYPE MaxGrade = findMax(grades[i]);
            GRADE_TYPE MinGrad = findMin(grades[i]);
            //��һ�����²���
            teams[j].years[i].grade = standardData(MaxGrade, MinGrad, teams[j].years[i].grade);
        }
    }
}

void init_CenterCluster(Team *CenterClusters, Team *teams) {
    for (int i = 0; i < MaxCenterClusterNumber; ++i) {
        printf("����������ѡ�����Ĵص�ID\n");
        int id;
        scanf("%d", &id);
        CenterClusters[i] = teams[id - 1];
        //�����޸�һ�����Ĵص�Id�������ӦA,B,C��
        CenterClusters[i].teamId = i + 1;
    }
}

void init_GradeData(Team *teams) {
    FILE *fp;
    if ((fp = fopen("D://forthdata//data.txt", "r")) == NULL) {
        printf("��ʧ��");
        exit(0);
    } else {
        for (int i = 0; i < MaxTeamNumber; ++i) {
            teams[i].teamId = i + 1;
            teams[i].centerClusterId = NullFlag;//��ʼ�����Ĵ�Ϊ��

            for (int j = 0; j < MaxYearNumber; ++j) {
                fscanf(fp, " %lf", &teams[i].years[j].grade);
            }
        }
    }
    standardGrade(teams);//��񻯴���
}

GRADE_TYPE caculateInstance(Team team, Team centerCluster) {
    GRADE_TYPE result;
    result = sqrt(pow(team.years[0].grade - centerCluster.years[0].grade, 2) +
                  pow(team.years[1].grade - centerCluster.years[1].grade, 2) +
                  pow(team.years[2].grade - centerCluster.years[2].grade, 2));
    //������λС������
    result = (int) (10000.0 * result + 0.5) / 10000.0;
    return result;
}

void init_Instance(Team *CenterClusters, Team *teams) {
    for (int i = 0; i < MaxTeamNumber; ++i) {
        for (int j = 0; j < MaxCenterClusterNumber; ++j) {
            teams[i].instances[j] = caculateInstance(teams[i], CenterClusters[j]);
        }
    }
}

int findCenterCluster(Team *CenterClusters, Team *teams) {
    INSTANCE_TYPE min = MaxNumber;
    int isUpdate = 0;
    for (int i = 0; i < MaxTeamNumber; ++i) {
        CENTERCLUSTERID_TYPE clusterId = 0;
        for (int j = 0; j < MaxCenterClusterNumber; ++j) {
            if (min > teams[i].instances[j]) {
                min = teams[i].instances[j];
                clusterId = CenterClusters[j].teamId;
            }
        }
        //��һ������
        if (teams[i].centerClusterId == NullFlag) {
            teams[i].centerClusterId = clusterId;
            isUpdate=1;
        } else if (teams[i].centerClusterId != clusterId) {
            //�ڶ�������
            teams[i].centerClusterId = clusterId;
            isUpdate = 1;//��ʾ�Ѿ�������Ҫ�ٴμ�����롣
        }
        min = MaxNumber;
    }
    return isUpdate;
}

int returnArrayLength(TEAMID_TYPE *ids) {
    int result = 0;
    for (int i = 0; i < MaxTeamNumber; ++i) {
        if (ids[i] != NullFlag) {
            result += 1;
        } else {
            break;
        }
    }
    return result;
}

void newNullArray(TEAMID_TYPE *idArray) {
    for (int i = 0; i < MaxTeamNumber; ++i) {
        idArray[i] = NullFlag;
    }
}

void findSameCluster(Team *teams, TEAMID_TYPE *idArray, CENTERCLUSTERID_TYPE id) {
    for (int i = 0; i < MaxTeamNumber; ++i) {
        if (teams[i].centerClusterId == id) {
            //��ѵ����˳�����idArray
            for (int j = 0; j < MaxTeamNumber; ++j) {
                if (idArray[j] == NullFlag) {
                    idArray[j] = teams[i].teamId;
                    break;
                }
            }
        }
    }
}

void caculateNewCenterCluster(Team *CenterClusters, Team *teams) {
    TEAMID_TYPE idArray[MaxTeamNumber];//��¼ͬһ�����ӵ�ID
    //��ʼ������
    newNullArray(idArray);
    //�����µ����ĵ�
    for (int k = 0; k < MaxCenterClusterNumber; ++k) {
        //�����Ȳ���ͬ���teamid
        findSameCluster(teams, idArray, k + 1);

        int length = returnArrayLength(idArray);
        //�����µľ���
        for (int i = 0; i < MaxCenterClusterNumber; ++i) {
            INSTANCE_TYPE newInstance = 0;
            for (int j = 0; j < length; ++j) {
                int teamId = idArray[j];
                newInstance += teams[teamId - 1].years[i].grade;
            }
            newInstance /= length;
            CenterClusters[k].years[i].grade = newInstance;
        }
        newNullArray(idArray);
    }
}

void k_means(Team *CenterClusters, Team *teams) {
    int count = 0;
    int isUpdate = 1;
    for (int i = 0; i < MaxTeamNumber; ++i) {
        if (isUpdate==1){
            init_Instance(CenterClusters, teams);//������ӵ����Ĵصľ���
            isUpdate=findCenterCluster(CenterClusters, teams);//���ൽ��Ӧ�����Ĵ�
            caculateNewCenterCluster(CenterClusters, teams);//�����µ����ľ���
            count++;
        }else{
            break;
        }
    }
    printf("k-means�㷨����������%d��\n",count);
    printResult(teams);
}
void printResult(Team *teams){
    for (int i = 0; i <MaxTeamNumber ; ++i) {
        idToTeamName(teams[i].teamId);
        idToClusterName(teams[i].centerClusterId);
    }
}
void idToClusterName(int id) {
    switch (id) {
        case 1:
            printf("A ");
            break;
        case 2:
            printf("B ");
            break;
        case 3:
            printf("C ");
            break;
    }
}

void idToTeamName(int id) {
    switch (id) {
        case 1:
            printf("1.�й�");
            break;
        case 2:
            printf("2.�ձ�");
            break;
        case 3:
            printf("3.����");
            break;
        case 4:
            printf("4.����");
            break;
        case 5:
            printf("5.ɳ��");
            break;
        case 6:
            printf("6.������");
            break;
        case 7:
            printf("7.������");
            break;
        case 8:
            printf("8.������");
            break;
        case 9:
            printf("9.���ȱ��˹̹");
            break;
        case 10:
            printf("10.̩��");
            break;
        case 11:
            printf("11.Խ��");
            break;
        case 12:
            printf("12.����");
            break;
        case 13:
            printf("13.����");
            break;
        case 14:
            printf("14.����");
            break;
        case 15:
            printf("15.ӡ��");
            break;
    }
}
