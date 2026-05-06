export interface ApiResponse<T> {
  success: boolean;
  data: T;
  message?: string;
  error?: {
    code: string;
    message: string;
  };
}

export interface PaginatedResponse<T> extends ApiResponse<T[]> {
  pagination?: {
    page: number;
    pageSize: number;
    totalCount: number;
    totalPages: number;
    hasNextPage: boolean;
    hasPreviousPage: boolean;
  };
}

export type UserRole = 'ORGANIZER' | 'AUTHOR' | 'REVIEWER' | 'PARTICIPANT' | 'ADMIN';

export interface User {
  id?: string;
  userId?: string;
  email: string;
  firstName?: string;
  lastName?: string;
  fullName?: string;
  affiliation?: string;
  role: UserRole;
  isActive: boolean;
  createdAt: string;
  updatedAt: string;
}

export interface Submission {
  id?: string;
  submissionId?: string;
  title: string;
  abstract: string;
  authors: string;
  status: string; // 'PENDING', 'UNDER_REVIEW', 'ACCEPTED', 'REJECTED'
  createdAt: string;
  updatedAt: string;
}

export interface Review {
  id?: string;
  reviewId?: string;
  submissionId: string;
  reviewerId: string;
  score: number;
  comments: string;
  status: string; // 'PENDING', 'COMPLETED'
  createdAt: string;
}

export interface Session {
  id?: string;
  sessionId?: string;
  title: string;
  description: string;
  startTime: string;
  endTime: string;
  location: string;
  capacity: number;
  chairId?: string;
}
