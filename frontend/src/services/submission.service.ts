import api from './api';
import type { ApiResponse, PaginatedResponse, Submission } from '../types/api';

const submissionService = {
  list: async (params?: any): Promise<PaginatedResponse<Submission>> => {
    return api.get('/submissions', { params });
  },

  getById: async (id: string): Promise<ApiResponse<Submission>> => {
    return api.get(`/submissions/${id}`);
  },

  create: async (data: any): Promise<ApiResponse<Submission>> => {
    if (data instanceof FormData) {
      return api.post('/submissions', data, {
        headers: { 'Content-Type': 'multipart/form-data' }
      });
    }
    return api.post('/submissions', data);
  },

  update: async (id: string, data: any): Promise<ApiResponse<Submission>> => {
    return api.put(`/submissions/${id}`, data);
  },

  delete: async (id: string): Promise<ApiResponse<null>> => {
    return api.delete(`/submissions/${id}`);
  },

  uploadFile: async (id: string, file: File): Promise<ApiResponse<any>> => {
    const formData = new FormData();
    formData.append('file', file);
    return api.post(`/submissions/${id}/files`, formData, {
      headers: { 'Content-Type': 'multipart/form-data' }
    });
  }
};

export default submissionService;
