import api from './api';
import type { ApiResponse, PaginatedResponse, Session } from '../types/api';

const sessionService = {
  list: async (params?: any): Promise<PaginatedResponse<Session>> => {
    return api.get('/sessions', { params });
  },

  getById: async (id: string): Promise<ApiResponse<Session>> => {
    return api.get(`/sessions/${id}`);
  },

  register: async (id: string): Promise<ApiResponse<any>> => {
    return api.post(`/sessions/${id}/register`);
  },

  unregister: async (id: string): Promise<ApiResponse<any>> => {
    return api.delete(`/sessions/${id}/register`);
  }
};

export default sessionService;
