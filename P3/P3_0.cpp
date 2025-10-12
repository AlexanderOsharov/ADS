#include <iostream>
#include <vector>

using namespace std;

using ll = long long;

const int CUTOFF = 64;

void naive_multiply(const vector<vector<ll>>& A, const vector<vector<ll>>& B,
                    vector<vector<ll>>& C) {
  int n = A.size();
  for (int i = 0; i < n; ++i) {
    for (int k = 0; k < n; ++k) {
      if (A[i][k] == 0) continue;
      for (int j = 0; j < n; ++j) {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }
}

void add_matrices(const vector<vector<ll>>& A, const vector<vector<ll>>& B,
                  vector<vector<ll>>& C, bool subtract = false) {
  int n = A.size();
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j)
      C[i][j] = A[i][j] + (subtract ? -B[i][j] : B[i][j]);
}

void strassen_multiply(const vector<vector<ll>>& A, const vector<vector<ll>>& B,
                       vector<vector<ll>>& C) {
  int n = A.size();
  if (n <= CUTOFF) {
    naive_multiply(A, B, C);
    return;
  }

  int h = n / 2;

  auto get_block = [&](const vector<vector<ll>>& M, int r, int c) {
    vector<vector<ll>> block(h, vector<ll>(h));
    for (int i = 0; i < h; ++i)
      for (int j = 0; j < h; ++j) block[i][j] = M[r + i][c + j];
    return block;
  };

  auto set_block = [&](vector<vector<ll>>& M, const vector<vector<ll>>& block,
                       int r, int c) {
    for (int i = 0; i < h; ++i)
      for (int j = 0; j < h; ++j) M[r + i][c + j] = block[i][j];
  };

  auto A11 = get_block(A, 0, 0);
  auto A12 = get_block(A, 0, h);
  auto A21 = get_block(A, h, 0);
  auto A22 = get_block(A, h, h);

  auto B11 = get_block(B, 0, 0);
  auto B12 = get_block(B, 0, h);
  auto B21 = get_block(B, h, 0);
  auto B22 = get_block(B, h, h);

  vector<vector<ll>> S1(h, vector<ll>(h));
  vector<vector<ll>> S2(h, vector<ll>(h));
  vector<vector<ll>> P1(h, vector<ll>(h));

  // M1 = (A11 + A22) * (B11 + B22)
  add_matrices(A11, A22, S1);
  add_matrices(B11, B22, S2);
  strassen_multiply(S1, S2, P1);

  // M2 = (A21 + A22) * B11
  add_matrices(A21, A22, S1);
  vector<vector<ll>> P2(h, vector<ll>(h));
  strassen_multiply(S1, B11, P2);

  // M3 = A11 * (B12 - B22)
  add_matrices(B12, B22, S1, true);
  vector<vector<ll>> P3(h, vector<ll>(h));
  strassen_multiply(A11, S1, P3);

  // M4 = A22 * (B21 - B11)
  add_matrices(B21, B11, S1, true);
  vector<vector<ll>> P4(h, vector<ll>(h));
  strassen_multiply(A22, S1, P4);

  // M5 = (A11 + A12) * B22
  add_matrices(A11, A12, S1);
  vector<vector<ll>> P5(h, vector<ll>(h));
  strassen_multiply(S1, B22, P5);

  // M6 = (A21 - A11) * (B11 + B12)
  add_matrices(A21, A11, S1, true);
  add_matrices(B11, B12, S2);
  vector<vector<ll>> P6(h, vector<ll>(h));
  strassen_multiply(S1, S2, P6);

  // M7 = (A12 - A22) * (B21 + B22)
  add_matrices(A12, A22, S1, true);
  add_matrices(B21, B22, S2);
  vector<vector<ll>> P7(h, vector<ll>(h));
  strassen_multiply(S1, S2, P7);

  // C11 = M1 + M4 - M5 + M7
  vector<vector<ll>> C11(h, vector<ll>(h));
  add_matrices(P1, P4, C11);
  add_matrices(C11, P5, C11, true);
  add_matrices(C11, P7, C11);

  // C12 = M3 + M5
  vector<vector<ll>> C12(h, vector<ll>(h));
  add_matrices(P3, P5, C12);

  // C21 = M2 + M4
  vector<vector<ll>> C21(h, vector<ll>(h));
  add_matrices(P2, P4, C21);

  // C22 = M1 - M2 + M3 + M6
  vector<vector<ll>> C22(h, vector<ll>(h));
  add_matrices(P1, P2, C22, true);
  add_matrices(C22, P3, C22);
  add_matrices(C22, P6, C22);

  set_block(C, C11, 0, 0);
  set_block(C, C12, 0, h);
  set_block(C, C21, h, 0);
  set_block(C, C22, h, h);
}

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int n;
  cin >> n;

  vector<vector<ll>> A(n, vector<ll>(n));
  vector<vector<ll>> B(n, vector<ll>(n));

  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j) cin >> A[i][j];

  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j) cin >> B[i][j];

  vector<vector<ll>> C(n, vector<ll>(n, 0));

  if (n == 1) {
    C[0][0] = A[0][0] * B[0][0];
  } else {
    strassen_multiply(A, B, C);
  }

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      cout << C[i][j] << (j + 1 == n ? '\n' : ' ');
    }
  }

  return 0;
}
