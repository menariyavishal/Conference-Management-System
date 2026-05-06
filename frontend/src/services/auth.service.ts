import api from './api';
import type { ApiResponse, User } from '../types/api';

export interface LoginResponse {
  token: string;
  userId?: string;
  id?: string;
  email: string;
  fullName: string;
  role: string;
}

const authService = {
  login: async (credentials: any): Promise<ApiResponse<LoginResponse>> => {
    return api.post('/auth/login', credentials);
  },

  register: async (userData: any): Promise<ApiResponse<User>> => {
    return api.post('/auth/register', userData);
  },

  logout: () => {
    localStorage.removeItem('token');
    localStorage.removeItem('user');
  },

  getCurrentUser: (): User | null => {
    const user = localStorage.getItem('user');
    return user ? JSON.parse(user) : null;
  }
};

export default authService;
